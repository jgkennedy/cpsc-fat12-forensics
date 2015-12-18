# FAT12 Forensics

Searches the root directory areas of a FAT12 filesystem image, recursively searching for files. It will attempt to recover deleted files by assuming adjacent free blocks in the FAT once held pointers to its data.

## Known Issues

At least one sector of deleted files will be recovered, even if that sector has
been overwritten by another file.
