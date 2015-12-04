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

int fdin, fdout;
char *file;
int filenum = 0;

unsigned long cluster2offset(unsigned short cluster) {
	return (cluster + 31) * 512;
}

unsigned short next_fat_cluster(unsigned short cluster) {
	unsigned long offset = 0x200 + (cluster/2)*3;
	unsigned long chunk = 0;
	memcpy(&chunk, &file[offset], 3);
	unsigned short next_cluster = 0;

	if (cluster % 2 == 0) {
		// cluster is even, read lower 12 bits of number
		next_cluster = (0x000FFF & chunk);
		// printf("next cluster is %x (%d)\n", next_cluster, next_cluster);
	} else {
		// cluster is odd, read upper 12 bits of number
		next_cluster = ((0xFFF000 & chunk) >> 12);
		// printf("next cluster is %x (%d)\n", next_cluster, next_cluster);
	}

	return next_cluster;
}

// entries are 32B
// clusters are 512B (enough to hold 16 directory entries)
void traverse_directory(unsigned long start, char *dir) {
	// printf("traversing 0x%lx\n", start);

	for (unsigned long offset = start; offset <= start + 512; offset += 32) {
		// No more valid entries in the table
		if (file[offset] == 0)
			return;

		unsigned short cluster;
		memcpy(&cluster, &file[offset+26], 2);

		unsigned long size = 0;
		memcpy(&size, &file[offset+28], 4);

		int deleted = 0;
		char filename[9];
		char extension[4];
		memset(filename, 0, sizeof filename);
		memset(extension, 0, sizeof extension);
		int position = 0, extposition = 0;

		for (int i = 0; i < 8; i++) {
			if (file[offset+i] == '\xE5') {
				deleted = 1;
				filename[position++] = '_';
			} else if (file[offset+i] != '\x20') {
				filename[position++] = file[offset+i];
			}
		}

		for (int i = 8; i < 12; i++) {
			if (file[offset+i] != '\x20') {
				extension[extposition++] = file[offset+i];
			}
		}

		// Check if entry is a directory (check 4th bit in 11th byte)
		if ((file[offset+11] >> 4) & 1) {
			// Don't traverse dot directories
			if (file[offset] != '.') {
				char new_dir[strlen(dir)+14];
				strcpy(new_dir, dir);
				strcat(new_dir, filename);
				strcat(new_dir, "/");
				traverse_directory(cluster2offset(cluster), new_dir);
			}
		} else {
			char filename_out[11];
			sprintf(filename_out, "file%d.%s", filenum++, extension);
			// printf("File Out: %s\n", filename_out);

			if ((fdout = open(filename_out, O_WRONLY | O_CREAT)) < 0) {
				printf("Can't open %s for writing", filename_out);
				exit(1);
			}

			printf("FILE\t");
			if (deleted)
				printf("DELETED");
			else
				printf("NORMAL");
			printf("\t%s%s.%s\t%lu\n", dir, filename, extension, size);
			// printf("finding next cluster for %d\n", cluster);

			if (cluster >= 0x002) {
				do {
					unsigned long size2write = size >= 512 ? 512 : size;
					write(fdout, &file[cluster2offset(cluster)], size2write);
					size -= size2write;
					cluster = next_fat_cluster(cluster);
				}
				while (cluster >= 0x002 && !(cluster >= 0xFF8 && cluster <= 0xFFF));
			}
			close(fdout);
		}
	}
}

int main(int argc, char *argv[]) {
	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		printf("Can't open %s for reading\n", argv[1]);
		return 1;
	}

	file = mmap(0, FLOPPYSIZE, PROT_READ, MAP_PRIVATE | MAP_FILE, fdin, 0);

	// Change to output directory
	if (chdir(argv[2]) != 0) {
		printf("Can't change directory to %s\n", argv[2]);
		return 1;
	}

	// Start traversing from the root directory
	for (int i = 0; i < 14; i++) {
		traverse_directory(0x2600 + 512*i, "/");
	}
}
