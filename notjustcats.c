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

int fdin; //, fdout;
char *file;

// entries are 32B
// clusters are 512B (enough to hold 16 directory entries)
void traverse_directory(unsigned short start) {
	printf("traversing 0x%x\n", start);
	for (unsigned short offset = start; offset < start + 512; offset += 32) {
		// No more valid entries in the table
		if (file[offset] == 0)
			return;

		char filename[12]; // 8 bytes for filename, 3 for ext, 1 for \0
		memcpy(filename, &file[offset], 11);
		filename[11] = (char) NULL;

		unsigned short cluster;
		memcpy(&cluster, &file[offset+26], 2);

		unsigned long size;
		memcpy(&size, &file[offset+28], 4);

		printf("Filename: %s\n", filename);
		printf("Cluster: %d\n", cluster);
		printf("Size: %ld\n", size);
	}
}

int main(int argc, char *argv[]) {
	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		printf("Can't open %s for reading", argv[1]);
		return 1;
	}

	file = mmap(0, FLOPPYSIZE, PROT_READ, MAP_PRIVATE | MAP_FILE, fdin, 0);

	// Start traversing from the root directory
	for (int i = 0; i < 14; i++) {
		traverse_directory(0x2600 + 512*i);
	}
}
