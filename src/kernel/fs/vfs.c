#include <rhino/fs/vfs.h>

binary_tree_t* mount_tree;
fs_node_t* fs_root;

mutex_t vfs_lock;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
  if(node->read != 0){
    return node->read(node, offset, size, buffer);
  } else {
    return 0;
  }
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
	if(node->write != 0) return node->write(node, offset, size, buffer);
	else return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write){
	UNUSED(read);
	UNUSED(write);
	if(node->open != 0) node->open(node);
	else return;
}
void close_fs(fs_node_t *node){
	if(node->close != 0) node->close(node);
	else return;
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index){
	if((node->flags & FS_DIRECTORY) && node->readdir){
		struct dirent* ret = node->readdir(node, index);
		return ret;
	} else return NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name){
	if((node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0) return node->finddir(node, name);
	else return 0;
}

void* vfs_mount(char* path, fs_node_t* local_root){
	if(!mount_tree || !path || path[0] != '/') return NULL;

	acquire_spinlock(&vfs_lock);

	local_root->refcount = -1;

	binary_tree_node_t* ret_val = NULL;

	char* p = strdup(path);
	char* i = p;

	uint32_t path_len = strlen(p);
	while(i < p + path_len){
		if(*i == '/') *i = '\0';
		i++;
	}

	p[path_len] = '\0';
	i = p + 1;

	binary_tree_node_t* root_node = mount_tree->root;

	if(*i == '\0'){
		struct vfs_entry* root = (struct vfs_entry*)root_node->value;
		if(root->file) kprint("unmount first or something");
		root->file = local_root;
		fs_root = local_root;
		ret_val = root_node;
	} else {
		kprint_err("UNIM");
	}

	release_spinlock(&vfs_lock);

	return ret_val;
}

void init_vfs(){
	mount_tree = binary_tree_create();
	struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));
	memset(root, 0, sizeof(struct vfs_entry));
	root->name = kmalloc(7);
	memcpy(root->name, "[root]\0", 7);

	binary_tree_set_root(mount_tree, root);
}

bool fnmatch(const char* wild, const char* tame, const uint32_t flags){
  	bool case_sense = (flags & FNM_NOCASE) ? false : true;
  	bool match = true;
  	char* last_wild = NULL;
  	char* last_tame = NULL;
  	char t, w;

  	while(1){
		t = *tame;
		w = *wild;
		if(!t || (t == '\0')){
	  		if(!w || (w == '\0')) break;
	  		else if(w == '*'){
				wild++;
				continue;
	  		} else if(last_tame){
				if(!(*last_tame) || (*last_tame == '\0')){
		 			match = false;
		  			break;
				}
			tame = last_tame++;
			wild = last_wild;
	  	}

	  	match = false;
	  	break;
		} else {
	  		if(!case_sense){
				if(t >= 'A' && t <= 'Z') t += ('a' - 'A');
				if(w >= 'A' && w <= 'Z') w += ('a' - 'A');
	  		}
	  
	  		if((t != w) && (w != '?')){
				if(w == '*'){
					last_wild = (char*)++wild;
					last_tame = (char*)tame;

					w = *wild;

					if(!w || (w == '\0')) break;
					continue;
				} else if(last_wild){
					if(last_wild != wild){
						wild = last_wild;
						w = *wild;
						if(!case_sense && (w >= 'A') && (w <= 'Z')) w += ('a' - 'A');
						if((t == w) || (t == '?')) wild++;
					}
					tame++;
					continue;
				} else {
					match = false;
					break;
				}
			}
		}
		tame++;
		wild++;
  	}
	
	return match;
}