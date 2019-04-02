#include <rhino/fs/fat32.h>

#include <rhino/arch/x86/timer.h>

uint32_t fat, first_usable_sector, start_lba;
uint32_t cluster_size;
struct fat32_bpb bpb;
fs_node_t* node;

/*static uint32_t fat32_calculate_serial(){
    time_t time = now();
    return ((((time.second << 8) | (100 / 2)) + ((time.month << 8) | (time.day))) + (((time.hour << 8) | (time.minute)) + time.year));
}*/

static uint64_t fat32_cluster_to_lba(uint32_t cluster){
    //return first_usable_sector + cluster * bpb.sectors_per_cluster - (2 * bpb.sectors_per_cluster);
    return ((cluster - 2) * bpb.sectors_per_cluster) + first_usable_sector;
}

static void fat32_read_cluster_raw(uint32_t cluster, void* buf){
    uint64_t lba = fat32_cluster_to_lba(cluster);
    uint32_t bytes = bpb.sectors_per_cluster * 512;

    read_fs(node, lba * 512, bytes, buf);
}

static uint32_t fat32_create_fat_cluster_chain(uint32_t start_cluster, uint32_t* arr){
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
                if((cluster & 0xF0000000) != 0){
                    debug_log("[FAT32]: Trying to access cluster above 28bits\n");
                    end_of_file = true;
                } else {
                    if(arr != NULL){
                        *arr = cluster;
                        arr++;
                    } 
                    
                    
                    clusters_added++;

                    uint32_t next_cluster = 0;
                    uint32_t fat_offset = cluster * 4;

                    read_fs(node, (fat * 512) + fat_offset, 4, (uint8_t*)&next_cluster);

                    if((next_cluster & 0xF0000000) != 0) {
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



uint32_t fat32_read_entry(struct fat32_directory_entry* entry, uint32_t offset, uint32_t len, void* buf){
    if(BIT_IS_SET(entry->attribute, FAT_DIRECTORY_ENTRY_ATTRIBUTE_DIRECTORY)){
        debug_log("[FAT32]: Can't read directory\n");
        return 0;
    }

    if(len > entry->file_size){
        debug_log("[FAT32]: Len is bigger than file size\n");
        return 0;
    }


    if((offset + len) > entry->file_size){
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
    if(end_offset > entry->file_size) final_size = entry->file_size - offset;


    

    uint8_t tmp_buf[cluster_size];
    uint8_t *dest_buf = buf;

    while(true){
        fat32_read_cluster_raw(entry_clusters[cluster_offset], tmp_buf);

        size_t bytes_to_copy = final_size - bytes_read;
        if(bytes_to_copy > cluster_size) bytes_to_copy = cluster_size;

        memcpy(&dest_buf[bytes_read],  &tmp_buf[curr_offset], bytes_to_copy);
        bytes_read += bytes_to_copy;

        if(bytes_read == final_size) break;

        curr_offset = 0;

        cluster_offset++;
        if(cluster_offset > entry_n_clusters){
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

void init_fat32(fs_node_t* file_node, uint64_t lba){
    ASSERT(sizeof(struct fat32_directory_entry) == 32);
    ASSERT(sizeof(struct fat32_info_sector) == 512);

    start_lba = lba;
    read_fs(file_node, start_lba * 512, sizeof(struct fat32_bpb), (uint8_t*)&bpb);
    node = file_node;

    char test[] = FAT32_SYSTEM_TYPE;
    if(memcmp(bpb.system_type, test, 8) != 0){
        debug_log("[FAT32]: Didn't detect FAT32 Systemtype\n");
        return;
    }

    struct fat32_info_sector fs_info;
    memset(&fs_info, 0, 512);
    read_fs(node, (lba + bpb.fs_info_sector_number) * 512, 512, (uint8_t*)&fs_info);

    if(fs_info.signature1 != FAT32_INFO_SECTOR_SIG1 || fs_info.signature2 != FAT32_INFO_SECTOR_SIG2 || fs_info.signature3 != FAT32_INFO_SECTOR_SIG3){
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
    if(bpb.number_of_fats < 1){
        debug_log("[FAT32]: Detected less than one fat?");
        return;
    }


    fat = start_lba + bpb.reserved_sectors;
    cluster_size = bpb.sectors_per_cluster * 512;

    uint32_t root_size = fat32_create_fat_cluster_chain(bpb.root_cluster_number, NULL);
    uint32_t root_array[root_size];
    fat32_create_fat_cluster_chain(bpb.root_cluster_number, root_array);

    for(uint32_t i = 0; i < root_size; i++) {
        uint8_t buf[cluster_size];
        memset(buf, 0, cluster_size);

        fat32_read_cluster_raw(root_array[i], buf);


        for(int x = 0; x < 512; x += 32){
            struct fat32_directory_entry* entry = (struct fat32_directory_entry*)(((uintptr_t)buf) + x);

            if(entry->file_name[0] == 0){
                break; // Last entry
            } 


            if((uint8_t)entry->file_name[0] == 0xE5 || (uint8_t)entry->file_name[0] == 0x05) continue; // Free entry

            if(entry->attribute == 0x0F){
                continue; // LFN Entry
            } 

            if(BIT_IS_SET(entry->attribute, FAT_DIRECTORY_ENTRY_ATTRIBUTE_DIRECTORY)){
                for(int j = 0; j < 8; j++){
                    if(entry->file_name[j] == ' ') break;
                    putchar(entry->file_name[j]);
                }
                putchar('\n');
            } else {
                for(int j = 0; j < 8; j++){
                    if(entry->file_name[j] == ' ') break;
                    putchar(entry->file_name[j]);
                }
                putchar('.');
                for(int j = 0; j < 3; j++) putchar(entry->file_extension[j]);
                putchar('\n');

                char bff[25];

                fat32_read_entry(entry, 6, 5, bff);

                for(int h = 0; h < 5; h++){
                    putchar(bff[h]);
                }


            }

    }

    }



    close_fs(file_node);
}


/*
static size_t fat32_read_file_raw(uint32_t start_cluster, uint32_t file_read_start, uint32_t file_read_length, void* buf){
    bool end_of_chain = false;
    uint32_t previous_fat_entry = start_cluster;

    uint32_t read_starting_cluster = (file_read_start + 512 - 1) / 512; // Fast ceil
    uint32_t read_starting_offset = 0;
    do
    {
        switch(previous_fat_entry)
        {
            case 0:         // Invalid fat entries
            case 1:
                end_of_chain = true;
                debug_log("[FAT32]: Invalid FAT chain value\n");
                break;

            case 0x0FFFFFF7:
                end_of_chain = true;
                debug_log("[FAT32]: Bad Cluster FAT chain value\n");
                break;

            case 0x0FFFFFF8: // Reserved fat entries, 0x0FFFFFF8 to 0x0FFFFFFE
            case 0x0FFFFFF9:
            case 0x0FFFFFFA:
            case 0x0FFFFFFB:
            case 0x0FFFFFFC:
            case 0x0FFFFFFD:
            case 0x0FFFFFFE:
                end_of_chain = true;
                debug_log("[FAT32]: Reserved FAT chain value\n");
                break;

            case 0x0FFFFFFF:
                end_of_chain = true;
                break;
        
            default:
                {
                    uint32_t fat_entry = 0;
                    uint32_t offset = fat + previous_fat_entry;
                    read_fs(node, offset, 4, (uint8_t*)&fat_entry);
                    fat_entry &= 0x0FFFFFFF;
                    previous_fat_entry = fat_entry;

                    read_starting_offset += (bpb.sectors_per_cluster * 512);
                }

                break;
        }
    } while (!end_of_chain && read_starting_offset < (read_starting_cluster * (bpb.sectors_per_cluster * 512)));
    
    // Read only file_read_size


    void* tmp_block = kmalloc(bpb.sectors_per_cluster * 512);
    uint32_t bytes_read = 0;

    do
    {
        switch (previous_fat_entry)
        {
            case 0:         // Invalid fat entries
            case 1:
                end_of_chain = true;
                debug_log("[FAT32]: Invalid FAT chain value\n");
                break;

            case 0x0FFFFFF7:
                end_of_chain = true;
                debug_log("[FAT32]: Bad Cluster FAT chain value\n");
                break;

            case 0x0FFFFFF8: // Reserved fat entries, 0x0FFFFFF8 to 0x0FFFFFFE
            case 0x0FFFFFF9:
            case 0x0FFFFFFA:
            case 0x0FFFFFFB:
            case 0x0FFFFFFC:
            case 0x0FFFFFFD:
            case 0x0FFFFFFE:
                end_of_chain = true;
                debug_log("[FAT32]: Reserved FAT chain value\n");
                break;

            case 0x0FFFFFFF:
                end_of_chain = true;
                break;
        
            default:
                fat32_read_cluster_raw(previous_fat_entry, tmp_block);
                
                size_t bytes_to_copy = file_read_length - bytes_read;
                if(bytes_to_copy > (bpb.sectors_per_cluster * 512)) bytes_to_copy = (bpb.sectors_per_cluster * 512);

                memcpy(buf, tmp_block, bytes_to_copy);
                bytes_read += bytes_to_copy;
                buf = (void*)(((uintptr_t)buf) + bytes_to_copy);

                if(bytes_read == file_read_length){
                    end_of_chain = true;
                    break;
                }

                uint32_t fat_entry = 0;
                uint32_t offset = fat + previous_fat_entry;
                read_fs(node, offset, 4, (uint8_t*)&fat_entry);
                fat_entry &= 0x0FFFFFFF;
                previous_fat_entry = fat_entry;

                break;
        }
    } while (!end_of_chain);

    kfree(tmp_block);
    return bytes_read;
}*/

/*uint32_t n_clusters = IDIV_CEIL(entry->file_size, (bpb.sectors_per_cluster * 512));//((entry->file_size + (bpb.sectors_per_cluster * 512) - 1) / (bpb.sectors_per_cluster * 512));
    uint32_t clusters[n_clusters];
    memset(clusters, 0, n_clusters * sizeof(uint32_t));

    uint32_t chain_length = fat32_create_fat_cluster_chain(entry_starting_cluster, clusters);

    if(n_clusters != chain_length) debug_log("[FAT32]: nclusters is not equal to chain_length\n");

    //uint32_t starting_cluster = offset / (bpb.sectors_per_cluster * 512);
    
    uint32_t bytes_read = 0;

    

    uint32_t clus_offset = offset / (bpb.sectors_per_cluster * 512);
    uint32_t first_offset = offset % (bpb.sectors_per_cluster * 512);

    int i = 0;
    uint32_t clus = clusters[i];
    uint32_t curr_offset = first_offset;

    while(clus_offset){
        if(clusters[clus_offset]){
            clus = clusters[clus_offset];
            clus_offset--;
        }
    }

    uint8_t tmp_buf[cluster_size];

    while(true){
        fat32_read_cluster_raw(clus, tmp_buf);

        size_t bytes_to_copy = len - bytes_read;
        if(bytes_to_copy > 512) bytes_to_copy = 512;

        void* dest = (void*)((uint8_t*)buf + bytes_read);

        memcpy(dest, &(tmp_buf[curr_offset]), bytes_to_copy);

        if(bytes_read == len) return bytes_read;

        curr_offset = 0;
        clus = clusters[i++];
    }*/