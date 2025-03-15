#include "program_header.h"

/*
    TAR archives:
        - they are (typically) organized in blocks of 512 bytes
*/

// After calling this function only necessary system calls will be allowed
// and others will be blocked
// int limit_system_calls() {
//     scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
//     if (ctx == NULL) {
//         perror("seccomp_init failed");
//         return 1;
//     }

//     seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
//     seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
//     seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
//     seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);
//     seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat), 0);

//     if (seccomp_load(ctx) < 0) {
//         perror("seccomp_load failed");
//         seccomp_release(ctx);
//         return 1;
//     }
//     seccomp_release(ctx);
//     return 0;
// }

int read_file(struct tar *tar_pointer, char *name, char **start, int *length) {

    // tar is already set to the starting entry of the TAR archive
    // The loop stops when the end of the tar archive is reached or it is an
    // empty entry

    for(; tar_pointer->name[0]; tar_pointer+=1+(*length+511)/512) {

        // "%o" is used to convert the size of the currently looked file
        // from an octal string to an intiger for it to be stored in the
        // length variable so it can be calculated in tar+=1+(*length+511)/512
        // to move to the next block of memory (file)

        sscanf( tar_pointer->size, "%o", length);
        if(!strcmp(tar_pointer->name,name)) {

            // start is set to point to the location where the file data starts
            // the file data can be text(.txt) or some code(.c)
            
            *start = (char*)(tar_pointer+1);
            return 1;
        }
    }
    return 0;
}

int display_file_properties(void *tar_obj, struct posix_header *header) {
    char *start;
    int length;
    if (strstr(header->name, ".save") == NULL && strstr(header->name, "~") == NULL)
    {
        if (strstr(header->name, ".txt") != NULL || strstr(header->name, ".c") != NULL
            || strstr(header->name, ".h") != NULL || strstr(header->name, "Makefile") != NULL) 
        {
            printf("---------------------------------\n");
            printf("\t%s\n\tContents:\n", header->name);
            printf("---------------------------------\n");

            if(read_file(tar_obj,header->name,&start,&length)) {
                for (int i = 0; i < length; i++) {
                    if (start[i-1] == '\n' || i == 0)
                        printf("\t\t");
                    printf("%c", start[i]);
                }
            }
            else {
                perror("Error while reading file");
                return -1;
            }

            printf("\n");
        }
    }
    return 0;
}

void make_permissions_readable(char *mode, char **permissions) {
    int position = 0;
    int lenght = strlen(mode);

    *permissions = malloc(32);
    for (position = 4; position < lenght; position++) {
        switch(mode[position]) {
            case '7':
                    strcat(*permissions,"rwx-");
                    break;
            case '6':
                    strcat(*permissions,"rw-");
                    break;
            case '5':
                    strcat(*permissions,"rx-");
                    break;
            case '4':
                    strcat(*permissions,"r-");
                    break;
            case '3':
                    strcat(*permissions,"wx-");
                    break;
            case '2':
                    strcat(*permissions,"w-");
                    break;
            case '1':
                    strcat(*permissions,"x-");
                    break;
            case '0':
                    strcat(*permissions,"--");
                    break;

            default: break;
        }
    }
}