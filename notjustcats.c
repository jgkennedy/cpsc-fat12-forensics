// Jeremy Wilder / Jesse Kennedy
// CPSC 322 - Prog 4

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FLOPPYSIZE 1474560

int main(int argc, char *argv[]){
	int fdin; //, fdout;
	char *file;

	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		printf("Can't open %s for reading", argv[1]);
		return 1;
	}

	file = mmap(0, FLOPPYSIZE, PROT_READ, MAP_PRIVATE | MAP_FILE, fdin, 0);

	char filename[12]; // 8 bytes for filename, 3 for ext, 1 for \0
	memcpy(filename, &file[0x2600], 11);
	filename[11] = NULL;

	unsigned short cluster;
	memcpy(&cluster, &file[0x2600+26], 2);

	unsigned long size;
	memcpy(&size, &file[0x2600+28], 4);

	printf("Filename: %s\n", filename);
	printf("Cluster: %d\n", cluster);
	printf("Size: %ld\n", size);
}
