#include <rhino/fs/fat32.h>

#include <rhino/arch/x86/timer.h>

uint32_t fat, first_usable_sector, start_lba;
uint32_t cluster_size, root_dir_n_clusters;
struct fat32_bpb bpb;
fs_node_t *node;

/*static uint32_t fat32_calculate_serial(){
    time_t time = now();
    return ((((time.second << 8) | (100 / 2)) + ((time.month << 8) | (time.day))) + (((time.hour << 8) | (time.minute)) + time.year));
}*/

static uint64_t fat32_cluster_to_lba(uint32_t cluster)
{
    //return first_usable_sector + cluster * bpb.sectors_per_cluster - (2 * bpb.sectors_per_cluster);
    return ((cluster - 2) * bpb.sectors_per_cluster) + first_usable_sector;
}

static void fat32_read_cluster_raw(uint32_t cluster, void *buf)
{
    uint64_t lba = fat32_cluster_to_lba(cluster);
    uint32_t bytes = bpb.sectors_per_cluster * 512;

    read_fs(node, lba * 512, bytes, buf);
}

static uint32_t fat32_create_fat_cluster_chain(uint32_t start_cluster, uint32_t *arr)
{
    bool end_of_file = false;
    uint32_t clusters_added = 0;
    uint32_t cluster = start_cluster;
    do
    {
        switch (cluster)
        {
        case 0:
        case 1:
            debug_log("[FAT32]: Invalid cluster number\n");
            return clusters_added;

        case 0x0FFFFFF7:
            debug_log("[FAT32]: Running across bad cluster\n");
            return clusters_added;

        case 0x0FFFFFF8:
        case 0x0FFFFFF9:
        case 0x0FFFFFFA:
        case 0x0FFFFFFB:
        case 0x0FFFFFFC:
        case 0x0FFFFFFD:
        case 0x0FFFFFFE:
            debug_log("[FAT32]: Reserved cluster value\n");
            return clusters_added;

        case 0x0FFFFFFF:
            return clusters_added;

        default:
            if ((cluster & 0xF0000000) != 0)
            {
                debug_log("[FAT32]: Trying to access cluster above 28bits\n");
                end_of_file = true;
            }
            else
            {
                if (arr != NULL)
                {
                    *arr = cluster;
                    arr++;
                }

                clusters_added++;

                uint32_t next_cluster = 0;
                uint32_t fat_offset = cluster * 4;

                read_fs(node, (fat * 512) + fat_offset, 4, (uint8_t *)&next_cluster);

                if ((next_cluster & 0xF0000000) != 0)
                {
                    debug_log("[FAT32]: Read cluster from fat which doesn't have top 4 bits clear, clearing\n");
                    next_cluster &= 0x0FFFFFFF;
                }

                cluster = next_cluster;
            }
            break;
        }
    } while (!end_of_file);

    return clusters_added;
}

static struct fat32_directory_entry fat32_get_entry_by_name(char *name)
{
    size_t path_length = strlen(name);
    char path[path_length + 1];
    strcpy(path, name);

    char *i = path;

    while (i < (path + path_length))
    {
        if (*i == '/')
            *i = '\0';
        i++;
    }

    path[path_length] = '\0';
    i = path;

    struct fat32_directory_entry *ret_val;
    uint32_t cluster = bpb.root_cluster_number;

    uint8_t directory_entry_buf[cluster_size];

    char *at = path;

    while (1)
    {
        if (at >= (path + path_length))
            break;

        uint32_t directory_entry_cluster_size = fat32_create_fat_cluster_chain(cluster, NULL);
        if (directory_entry_cluster_size == 0)
            goto couldnt_find;
        uint32_t directory_entry_cluster_array[directory_entry_cluster_size];
        fat32_create_fat_cluster_chain(cluster, directory_entry_cluster_array);

        for (uint32_t directory_entries_cluster_index = 0; directory_entries_cluster_index < directory_entry_cluster_size; directory_entries_cluster_index++)
        {
            memset(directory_entry_buf, 0, cluster_size);

            fat32_read_cluster_raw(directory_entry_cluster_array[directory_entries_cluster_index], directory_entry_buf);

            for (uint32_t directory_entry_index = 0; directory_entry_index < cluster_size; directory_entry_index += 32)
            {
                struct fat32_directory_entry *entry = (struct fat32_directory_entry *)(((uintptr_t)directory_entry_buf) + directory_entry_index);

                if (entry->file_name[0] == 0)
                {

                    goto couldnt_find;
                }

                if ((uint8_t)entry->file_name[0] == 0xE5 || (uint8_t)entry->file_name[0] == 0x05)
                    continue; // Free entry

                if (entry->attribute == 0x0F)
                {
                    continue; // LFN Entry
                }

                if (BIT_IS_SET(entry->attribute, FAT_DIRECTORY_ENTRY_ATTRIBUTE_DIRECTORY))
                {
                    if (memcmp(at, entry->file_name, MIN(strlen(at), sizeof(entry->file_name))) == 0)
                    {
                        cluster = ((entry->starting_cluster_high << 16) | (entry->starting_cluster_low));

                        ret_val = entry;

                        goto exit_loop;
                    }
                    continue;
                }
                else
                {

                    char *at_index = at;
                    char *file_name = at;
                    char *file_extension = NULL;
                    while (at_index < (at + strlen(at)))
                    {
                        if (*at_index == '.')
                        {
                            *at_index = '\0';
                            file_extension = at_index + 1;
                            break;
                        }

                        at_index++;
                    }

                    if (!file_extension)
                        continue;

                    if (memcmp(file_name, entry->file_name, strlen(file_name)) == 0)
                    {
                        if (memcmp(file_extension, entry->file_extension, strlen(file_extension)) == 0)
                        {

                            cluster = ((entry->starting_cluster_high << 16) | (entry->starting_cluster_low));

                            ret_val = entry;

                            goto found;
                        }
                    }

                    continue;
                }
            }

            goto couldnt_find;
        }

    exit_loop:

        at = at + strlen(at) + 1;
    }

found:;
    struct fat32_directory_entry ret;
    memcpy(&ret, ret_val, sizeof(struct fat32_directory_entry));

    return ret;
couldnt_find:
    debug_log("[FAT32]: Couldn't find file\n");
    struct fat32_directory_entry error_ret;
    memset(&error_ret, 0, sizeof(struct fat32_directory_entry));
    return error_ret;
}

uint32_t fat32_read_entry(struct fat32_directory_entry *entry, uint32_t offset, uint32_t len, void *buf)
{
    if (BIT_IS_SET(entry->attribute, FAT_DIRECTORY_ENTRY_ATTRIBUTE_DIRECTORY))
    {
        debug_log("[FAT32]: Can't read directory\n");
        return 0;
    }

    if (len > entry->file_size)
    {
        debug_log("[FAT32]: Len is bigger than file size\n");
        return 0;
    }

    if ((offset + len) > entry->file_size)
    {
        debug_log("[FAT32]: offset + len is bigger than file size\n");
        return 0;
    }

    uint32_t entry_starting_cluster = ((entry->starting_cluster_high << 16) | (entry->starting_cluster_low));
    uint32_t entry_n_clusters = fat32_create_fat_cluster_chain(entry_starting_cluster, NULL);
    uint32_t entry_clusters[entry_n_clusters];
    fat32_create_fat_cluster_chain(entry_starting_cluster, entry_clusters);

    uint32_t cluster_offset = offset / cluster_size;
    uint32_t first_offset = offset % cluster_size;

    uint32_t bytes_read = 0;
    uint32_t curr_offset = first_offset;

    uint32_t end_offset = offset + len;
    uint32_t final_size = len;
    if (end_offset > entry->file_size)
        final_size = entry->file_size - offset;

    uint8_t tmp_buf[cluster_size];
    uint8_t *dest_buf = buf;

    while (true)
    {
        fat32_read_cluster_raw(entry_clusters[cluster_offset], tmp_buf);

        size_t bytes_to_copy = final_size - bytes_read;
        if (bytes_to_copy > cluster_size)
            bytes_to_copy = cluster_size;

        memcpy(&dest_buf[bytes_read], &tmp_buf[curr_offset], bytes_to_copy);
        bytes_read += bytes_to_copy;

        if (bytes_read == final_size)
            break;

        curr_offset = 0;

        cluster_offset++;
        if (cluster_offset > entry_n_clusters)
        {
            debug_log("[FAT32] Trying to read from cluster outside of fat chain\n");
            return 0;
        }
    }

    time_t time = now();

    uint8_t day = (time.day & 0xF) + 1;
    uint8_t month = ((time.month & 0xF) + 1) << 4;
    uint8_t year = (time.year - 1980) << 8;
    uint16_t date_last_accessed = (day | month | year);

    entry->date_last_accessed = date_last_accessed;

    return bytes_read;
}

void init_fat32(fs_node_t *file_node, uint64_t lba)
{
    ASSERT(sizeof(struct fat32_directory_entry) == 32);
    ASSERT(sizeof(struct fat32_info_sector) == 512);

    start_lba = lba;
    read_fs(file_node, start_lba * 512, sizeof(struct fat32_bpb), (uint8_t *)&bpb);
    node = file_node;

    char test[] = FAT32_SYSTEM_TYPE;
    if (memcmp(bpb.system_type, test, 8) != 0)
    {
        debug_log("[FAT32]: Didn't detect FAT32 Systemtype\n");
        return;
    }

    struct fat32_info_sector fs_info;
    memset(&fs_info, 0, 512);
    read_fs(node, (lba + bpb.fs_info_sector_number) * 512, 512, (uint8_t *)&fs_info);

    if (fs_info.signature1 != FAT32_INFO_SECTOR_SIG1 || fs_info.signature2 != FAT32_INFO_SECTOR_SIG2 || fs_info.signature3 != FAT32_INFO_SECTOR_SIG3)
    {
        debug_log_number_hex(fs_info.signature1);
        debug_log("\n");
        debug_log_number_hex(fs_info.signature2);
        debug_log("\n");
        debug_log_number_hex(fs_info.signature3);
        debug_log("\n");
        debug_log("[FAT32]: Info Sector signatures invalid\n");
        return;
    }

    first_usable_sector = start_lba + bpb.reserved_sectors + (bpb.number_of_fats * bpb.sectors_per_fat); //start_lba + bpb.reserved_sectors + (bpb.number_of_fats * bpb.sectors_per_fat);
    if (bpb.number_of_fats < 1)
    {
        debug_log("[FAT32]: Detected less than one fat?");
        return;
    }

    fat = start_lba + bpb.reserved_sectors;
    cluster_size = bpb.sectors_per_cluster * 512;

    struct fat32_directory_entry e = fat32_get_entry_by_name("TEST.TXT");
    (void)(e);

    root_dir_n_clusters = fat32_create_fat_cluster_chain(bpb.root_cluster_number, NULL);
}

/*static uint32_t fat32_read(fs_node_t *node, uint64_t offset, uint32_t size, uint8_t *buffer)
{
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length)
        return 0;
    if (offset + size > header.length)
        size = header.length - offset;
    memcpy(buffer, (uint8_t *)(uint32_t)(header.offset + offset), size);
    return size;
}

static struct dirent *fat32_readdir(fs_node_t *node, uint32_t index)
{
    UNUSED(node);
    if (index >= nroot_nodes)
        return 0;
    strcpy(dirent->name, root_nodes[index].name);
    dirent->name[strlen(root_nodes[index].name)] = 0; // Make sure the string is NULL-terminated.
    dirent->inode = root_nodes[index].inode;
    return dirent;
}

static fs_node_t *fat32_finddir(fs_node_t *node, char *name)
{
    debug_log("[FAT32]: finddir: ");
    debug_log(name);
    debug_log("\n");

    return 0;
}*/