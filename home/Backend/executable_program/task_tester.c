#include "program_header.h"

size_t octal_to_decimal(const char *octal_str) {
    return strtoul(octal_str, NULL, 8);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.tar>\n", argv[0]);
        return 1;
    }

    if (setuid(geteuid()) != 0) {
        perror("Failed to drop privileges");
        exit(1);
    }

    struct stat fileStat;
    if (stat(argv[1], &fileStat) < 0) {
        perror("Error getting file information");
        return 1;
    }

    // S_IRUSR and S_IWUSR are only going to give the read and write permissions to the Owner
    // S_IRGRP and S_IROTH are giving group and other only read permissions
    int fd = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    printf("\n\n");
    printf("Opening %s...\n", argv[1]);

    struct stat sb;
    printf("Getting the size of the file...\n");
    if (fstat(fd, &sb) == -1) {
        perror("Error getting the file size");
        close(fd);
        return 1;
    }
    size_t tar_file_size = sb.st_size;

    printf("Mapping the file into memory...\n");
    void *tar_obj = mmap(NULL, tar_file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (tar_obj == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }
    close(fd);

    printf("\nPrinting the directory and its contents:\n");
    size_t offset = 0;
    size_t offset_for_printing_content = 0;
    int width = 35;

    while (offset < tar_file_size) {
        char buffer[256] = "\t";
        char *permissions = NULL;
        struct posix_header *header = (struct posix_header *)((char *)tar_obj + offset);

        if (header->name[0] == '\0') break;

        strcat(buffer, header->name);
        if (header->typeflag == '5') strcat(buffer, " (directory)");
        else strcat(buffer, " (file)");
        
        printf("%s%*s", buffer, (int)(width - strlen(buffer)), "");
        make_permissions_readable(header->mode, &permissions);
        printf("- permissions: %s \n", permissions);

        size_t file_size = octal_to_decimal(header->size);
        size_t block_size = ((file_size + 511) / 512) * 512;
        if (offset == 0)
            offset_for_printing_content += 512 + block_size;
        offset += 512 + block_size;
    }

    putchar('\n');
    offset = offset_for_printing_content;
    while (offset < tar_file_size) {
        struct posix_header *header = (struct posix_header *)((char *)tar_obj + offset);
        if (header->name[0] == '\0') break;
        if (header->typeflag != '5') {
            if (display_file_properties(tar_obj, header) == -1) {
                munmap(tar_obj, tar_file_size);
                return 1;
            }
        }

        size_t file_size = octal_to_decimal(header->size);
        size_t block_size = ((file_size + 511) / 512) * 512;
        offset += 512 + block_size;
    }

    munmap(tar_obj, tar_file_size);
    return 0;
}