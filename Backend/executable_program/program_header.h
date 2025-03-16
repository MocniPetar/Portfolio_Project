#ifndef __PROGRAM_HEADER_FILE__
#define __PROGRAM_HEADER_FILE__

#include <stdio.h> 
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h> 
#include <string.h> 
#include <unistd.h>
// #include <seccomp.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

extern struct tar {
  char name[100];
  char _unused[24];
  char size[12];
  char _padding[376];
} *tar;

struct posix_header {
    char name[100];      // File name
    char mode[8];        // Permissions
    char uid[8];         // User ID
    char gid[8];         // Group ID
    char size[12];       // File size in octal
    char mtime[12];      // Modification time
    char chksum[8];      // Checksum for header
    char typeflag;       // File type
    char linkname[100];  // Target file for symlinks
    char magic[6];       // indicates USTAR format
    char version[2];     // USTAR version
    char uname[32];      // Username
    char gname[32];      // Group name
    char devmajor[8];    // Major device number
    char devminor[8];    // Minor device number
    char prefix[155];    // Prefix for long names
};

// int limit_system_calls();
int read_file(struct tar *, char *, char **, int *);
int display_file_properties(void *, struct posix_header *);
void make_permissions_readable(char *, char **);

#endif