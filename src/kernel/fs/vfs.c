#include <rhino/fs/vfs.h>

binary_tree_t* mount_tree;
fs_node_t* fs_root;

mutex_t vfs_lock, refcount_lock;


static struct dirent* readdir_mapper(fs_node_t* node, uint32_t index){
	binary_tree_node_t* d = (binary_tree_node_t*)node->device;
	if(!d) return NULL;
	if(index == 0){
		struct dirent* dir = kmalloc(sizeof(struct dirent));
		strcpy(dir->name, ".");
		dir->inode = 0;
		return dir;
	} else if(index == 1){
		struct dirent* dir = kmalloc(sizeof(struct dirent));
		strcpy(dir->name, "..");
		dir->inode = 1;
		return dir;
	}

	index -= 2;
	uint32_t i = 0;
	for(linked_list_node_t* child = d->children->head; child != NULL; child = child->next){
		if(i == index){
			binary_tree_node_t* tchild = (binary_tree_node_t*)child->data;
			struct vfs_entry* n = (struct vfs_entry*)tchild->value;
			struct dirent* dir = kmalloc(sizeof(struct dirent));

			size_t len = strlen(n->name) + 1;
			memcpy(&dir->name, n->name, (256 < len) ? (256) : (len));
			dir->inode = i;
			return dir;
		}
		i++;
	}

	return NULL;
}

static fs_node_t* vfs_mapper(){
	fs_node_t* fnode = kmalloc(sizeof(fs_node_t));
	memset(fnode, 0x00, sizeof(fs_node_t));
	fnode->mask = 0555;
	fnode->flags = FS_DIRECTORY;
	fnode->readdir = readdir_mapper;
	
	return fnode;
}

void map_vfs_directory(char* c){
	fs_node_t* f = vfs_mapper();
	struct vfs_entry* e = vfs_mount(c, f);
	if(strlen(c) == 1) f->device = mount_tree->root; //strcmp(c, "/") == 0
	else f->device = e;

	strcpy(f->name, c);
}

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
  if(node->read)return node->read(node, offset, size, buffer);
  else return 0;
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer){
	if(node->write) return node->write(node, offset, size, buffer);
	else return 0;
}

void open_fs(fs_node_t *node, uint32_t flags){
	if(!node) return;

	if(node->refcount >= 0){
		acquire_spinlock(&refcount_lock);
		node->refcount++;
		release_spinlock(&refcount_lock);
	}
	
	if(node->open) node->open(node, flags);
	else return;
}
void close_fs(fs_node_t *node){

	if(node == fs_root) return;
	if(!node) return;

	if(node->refcount == -1) return;

	acquire_spinlock(&refcount_lock);

	node->refcount--;
	if(node->refcount == 0){
		if(node->close) node->close(node);
		kfree(node);
	}

	release_spinlock(&refcount_lock);
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index){
	if((node->flags & FS_DIRECTORY) && node->readdir){
		struct dirent* ret = node->readdir(node, index);
		return ret;
	} else return NULL;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name){
	if((node->flags & 0x7) == FS_DIRECTORY && node->finddir) return node->finddir(node, name);
	else return 0;
}

uint32_t readlink_fs(fs_node_t* node, char* buf, size_t size){
	if(node->readlink) return node->readlink(node, buf, size);
	else return 0;
}

char* canonicalize_path(char* cwd, char* input){
	linked_list_t* out = linked_list_create();

	if(strlen(input) && input[0] != PATH_SEPARATOR){
		char* path = kmalloc((strlen(cwd) + 1) * sizeof(char));
		memcpy(path, cwd, strlen(cwd) + 1);

		char* pch;
		char* save;
		pch = strtok_r(path, PATH_SEPARATOR_STRING, &save);
		while(pch != NULL){
			char* s = kmalloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);

			linked_list_insert(out, s);
			pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
		}
		kfree(path);
	}

	

	char* path = kmalloc(sizeof(char) * (strlen(input) + 1));
	memcpy(path, input, strlen(input) + 1);

	char* pch;
	char* save;
	pch = strtok_r(path, PATH_SEPARATOR_STRING, &save);
	while(pch != NULL){
		if(strcmp(pch, PATH_UP) == 0){
			linked_list_node_t* n = linked_list_pop(out);
			if(n){
				kfree(n->data);
				kfree(n);
			}
		} else if (strcmp(pch, PATH_DOT) == 0){
			; // NOP
		} else {
			char* s = kmalloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			linked_list_insert(out, s);
		}
		pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
	}

	

	kfree(path);

	

	size_t size = 0;
	for(linked_list_node_t* i = out->head; i != NULL; i = i->next) size += strlen(i->data) + 1;

	char* output = kmalloc(sizeof(char) * (size + 1));
	char* output_offset = output;
	if(size == 0){
		output = krealloc(output, sizeof(char) * 2);
		output[0] = PATH_SEPARATOR;
		output[1] = '\0';
	} else {
		for(linked_list_node_t* i = out->head; i != NULL; i = i->next){
			output_offset[0] = PATH_SEPARATOR;
			output_offset++;
			memcpy(output_offset, i->data, strlen(i->data) + 1);
			output_offset += strlen(i->data);
		}
	}

	

	linked_list_destroy(out);
	
	linked_list_free(out);
	kfree(out);
	
	return output;
}

void* vfs_mount(char* path, fs_node_t* local_root){
	if(!mount_tree || !path || path[0] != PATH_SEPARATOR) return NULL;

	acquire_spinlock(&vfs_lock);

	local_root->refcount = -1;

	binary_tree_node_t* ret_val = NULL;

	char* p = strdup(path);
	char* i = p;

	size_t path_len = strlen(p);

	//vfs_split_path(i);

	while(i < p + path_len){
		if(*i == PATH_SEPARATOR) *i = '\0';
		i++;
	}

	p[path_len] = '\0';
	i = p + 1;

	binary_tree_node_t* root_node = mount_tree->root;

	if(*i == '\0'){
		struct vfs_entry* root = (struct vfs_entry*)root_node->value;
		if(root->file){
			debug_log("[VFS]: Path already mounted unmount first\n");
			return NULL;
		}
		root->file = local_root;
		fs_root = local_root;
		ret_val = root_node;
	} else {
		binary_tree_node_t* node = root_node;
		char* at = i;
		while(1){
			if(at >= (p + path_len)) break;

			bool found = false;
			for(linked_list_node_t* child = node->children->head; child != NULL; child = child->next){
				binary_tree_node_t* tchild = (binary_tree_node_t*)child->data;
				struct vfs_entry* ent = (struct vfs_entry*)tchild->value;
				if(strcmp(ent->name, at) == 0){
					found = true;
					node = tchild;
					ret_val = node;
					break;
				}
			}
			if(!found){
				struct vfs_entry* entry = kmalloc(sizeof(struct vfs_entry));
				entry->name = strdup(at);
				entry->file = NULL;
				entry->device = NULL;
				entry->fs_type = NULL;
				node = binary_tree_node_insert_child(mount_tree, node, entry);
			}
			at = at + strlen(at) + 1;
		}
		struct vfs_entry* entry = (struct vfs_entry*)node->value;
		if(entry->file){
			debug_log("[VFS]: Path already mounted unmount first\n");
		} 
		entry->file = local_root;
		ret_val = node;
	}

	kfree(p);

	release_spinlock(&vfs_lock);

	return ret_val;
}

fs_node_t* get_mount_point(char* path, uint32_t path_depth, char** outpath, uint32_t* outdepth){
	size_t depth;
	for(depth = 0; depth <= path_depth; ++depth) path += strlen(path) + 1;
	fs_node_t* last = fs_root;
	binary_tree_node_t* node = mount_tree->root;

	char* at = *outpath;
	uint32_t _depth = 1;
	uint32_t _tree_depth = 0;
	while(1){
		if(at >= path) break;

		bool found = false;
		for(linked_list_node_t* i = node->children->head; i != NULL; i = i->next){
			binary_tree_node_t* tchild = (binary_tree_node_t*)i->data;
			struct vfs_entry* ent = (struct vfs_entry*)tchild->value;
			if(strcmp(ent->name, at) == 0){
				found = true;
				node = tchild;
				at = at + strlen(at) + 1;
				if(ent->file){
					_tree_depth = _depth;
					last = ent->file;
					*outpath = at;
				}
				break;
			}
		}
		if(!found) break;
		_depth++;
	}
	*outdepth = _tree_depth;
	if(last){
		fs_node_t* last_clone = kmalloc(sizeof(fs_node_t));
		memcpy(last_clone, last, sizeof(fs_node_t));
		return last_clone;
	}
	return last;
}

fs_node_t* kopen_recursive(char *filename, uint32_t flags, uint32_t symlink_depth, char *relative_to){
	if(!filename) return NULL;

	char* path = canonicalize_path(relative_to, filename);

	size_t path_len = strlen(path);

	if(path_len == 1){
		fs_node_t* root_clone = kmalloc(sizeof(fs_node_t));
		memcpy(root_clone, fs_root, sizeof(fs_node_t));
		kfree(path);
		open_fs(root_clone, flags);
		return root_clone;
	}
	

	char* path_offset = path;
	uint32_t path_depth = 0;
	while(path_offset < (path + path_len)){
		if(*path_offset == PATH_SEPARATOR){
			*path_offset = '\0';
			path_depth++;
		}
		path_offset++;
	}
	path[path_len] = '\0';
	path_offset = path + 1;

	

	uint32_t depth = 0;

	fs_node_t* node_ptr = get_mount_point(path, path_depth, &path_offset, &depth);
	if(!node_ptr) return NULL;
	
	do {
		if((node_ptr->flags & FS_SYMLINK) && !((flags & O_NOFOLLOW) && (flags & O_PATH) && depth == path_depth)){
			if((flags & O_NOFOLLOW) && depth == (path_depth - 1)){
				debug_log("[VFS]: kopen_recursive Refusing to follow final entry with O_NOFOLLOW\n");
				kfree((void*)path);
				kfree(node_ptr);
				return NULL;
			}
			if(symlink_depth >= MAX_SYMLINK_DEPTH){
				debug_log("[VFS]: kopen_recursive reached max symlink depth\n");
				kfree((void*)path);
				kfree(node_ptr);
				return NULL;
			}

			char symlink_buf[MAX_SYMLINK_SIZE];
			int len = readlink_fs(node_ptr, symlink_buf, sizeof(symlink_buf));
			if(len < 0){
				debug_log("[VFS]: kopen_recursive got error opening symlink\n");
				kfree((void*)path);
				kfree(node_ptr);
				return NULL;
			}
			if(symlink_buf[len] != '\0'){
				debug_log("[VFS]: kopen_recursive readlink doesn't end in NULL\n");
				kfree((void*)path);
				kfree(node_ptr);
				return NULL;
			}

			fs_node_t* old_node_ptr = node_ptr;

			char* relpath = kmalloc(path_len + 1);
			char* ptr = relpath;
			memcpy(relpath, path, path_len + 1);
			for(uint32_t i = 0; depth && i < (depth - 1); i++){
				while(*ptr != '\0') ptr++;
				*ptr = PATH_SEPARATOR;
			}
			node_ptr = kopen_recursive(symlink_buf, 0, symlink_depth + 1, relpath);
			kfree(relpath);
			kfree(old_node_ptr);
			if(!node_ptr){
				debug_log("[VFS]: kopen_recursive failed to open symlink path\n");
				kfree((void*)path);
				return NULL;
			}
		}
		if(path_offset >= (path + path_len)){
			kfree(path);
			open_fs(node_ptr, flags);
			return node_ptr;
		}
		if(depth == path_depth){
			open_fs(node_ptr, flags);
			kfree((void*)path);
			return node_ptr;
		}

		fs_node_t* node_next = finddir_fs(node_ptr, path_offset);
		kfree(node_ptr);
		node_ptr = node_next;
		if(!node_ptr){
			kfree((void*)path);
			debug_log("[VFS]: Could not find file\n");
			return NULL;
		}
		path_offset += strlen(path_offset) + 1;
		++depth;
	} while(depth < (path_depth + 1));
	
	debug_log("[VFS]: kopen_recursive file not found\n");
	kfree((void*)path);

	return NULL;
}

fs_node_t* kopen(char* path, uint32_t flags){
	acquire_spinlock(&vfs_lock);

	fs_node_t* ret = kopen_recursive(path, flags, 0, (char*)(task_get_working_directory(get_running_task())));
	release_spinlock(&vfs_lock);
	return ret;
}

void init_vfs(){
	acquire_spinlock(&vfs_lock);

	mount_tree = binary_tree_create();
	struct vfs_entry* root = kmalloc(sizeof(struct vfs_entry));
	memset(root, 0, sizeof(struct vfs_entry));
	root->name = strdup("[root]");
	root->fs_type = NULL;
	root->device = NULL;

	binary_tree_set_root(mount_tree, root);

	release_spinlock(&vfs_lock);
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