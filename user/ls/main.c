#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void main(int argc, char* argv[]){
	if(argc > 2){
		printf("read: to many arguments use -h or --help for help\n");
		return;
	}

	if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
		printf("-----------------------------------------------\n");
		printf("  ls: lists files\n");
		printf("    or type -h or --help for this screen\n");
		printf("-----------------------------------------------\n");
		return;
	}

	bool colour = false;

	if(strcmp(argv[1], "--colour") == 0 || strcmp(argv[2], "--colour") == 0) colour = true;

	int i = 0;

	char buf[128] = "";
	getcwd(buf, 128);

	int fd = open(buf);

	struct dirent *node = 0;
	while ((node = readdir(fd, i)) != 0)
	{
		if(colour){
			int file = open(node->name);
			struct stat st; 
			fstat(file, &st);
			if(S_ISDIR(st.st_mode)) syscall(15, 1, 0, 0);
			else syscall(15, 2, 0, 0);
		}
		printf(node->name);
		printf("   ");
		i++;
	}

	printf("\n");

	close(fd);
}