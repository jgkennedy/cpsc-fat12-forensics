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

unsigned long cluster2offset(unsigned short cluster) {
	return (cluster + 31) * 512;
}

// entries are 32B
// clusters are 512B (enough to hold 16 directory entries)
void traverse_directory(unsigned long start) {
	printf("traversing 0x%lx\n", start);
	for (unsigned long offset = start; offset <= start + 512; offset += 32) {
		// No more valid entries in the table
		if (file[offset] == 0)
			return;

		unsigned short cluster;
		memcpy(&cluster, &file[offset+26], 2);

		// Check if entry is a directory
		if ((file[offset+11] >> 4) & 1) {
			// Don't traverse dot directories
			if (file[offset] != '.')
				traverse_directory(cluster2offset(cluster));
		} else {
			int deleted = 0;
			char filename[12]; // 8 bytes for filename, 3 for ext, 1 for \0
			memset(filename, 0, sizeof filename);

			int position = 0;
			for (int i = 0; i < 11; i++) {
				if (i == 8) {
					filename[position++] = '.';
				}
				if (file[offset+i] == '\xE5') {
					deleted = 1;
					filename[position++] = '_';
				} else if (file[offset+i] != '\x20') {
					filename[position++] = file[offset+i];
				}

			}

			unsigned long size;
			memcpy(&size, &file[offset+28], 4);

			printf("\nFilename: %s\n", filename);
			printf("Cluster: %hu\n", cluster);
			printf("Size: %lu\n", size);
			if (deleted)
				printf("DELETED\n");
		}
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
