// Jeremy Wilder / Jesse Kennedy
// CPSC 322 - Prog 4

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FLOPPYSIZE 1474560

int main(int argc, char *argv[]){
	int fdin, fdout;
	char *file;

	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		printf("Can't open %s for reading", argv[1]);
		return 1;
	}

	file = mmap(0, FLOPPYSIZE, PROT_READ, MAP_PRIVATE | MAP_FILE, fdin, 0);
	printf("first character in file: %c.", file[0]);
}
