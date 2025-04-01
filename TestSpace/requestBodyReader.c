#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char **argv) 
{
    if (argc < 2) {
        printf("Please enter the file name when executing program...\n");
        exit(1);
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file...\n");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char buffer[file_size + 1];
    fread(buffer, file_size, 1, file);
    buffer[file_size] = '\0';
    
    size_t content_length = 0;
    char *content_length_header = strstr(buffer, "Content-Length:");
    if (content_length_header) {
        sscanf(content_length_header, "Content-Length: %zd", &content_length);
    }

    char *body = (char *)malloc(content_length * sizeof(char));
    if (body == NULL) { fprintf(stderr, "malloc failed...\n"); exit(-1); }

    char *fileName = strstr(buffer, "filename=");
    char* crlf_location_before_data = NULL;
    char* crlf_location_after_data = NULL;
    if (fileName) {
        crlf_location_before_data = strstr(fileName, "\r\n\r\n");

        if(crlf_location_before_data) {
            crlf_location_after_data = strstr(crlf_location_before_data + 4, "\r\n\r\n");
        }

        if (crlf_location_after_data) {
            size_t size_of_data = strlen(crlf_location_before_data) - strlen(crlf_location_after_data - 4);
            memcpy(body, crlf_location_before_data + 4, size_of_data);
            body[size_of_data + 1] = '\0';
        }
    }
    else {
        memcpy(body, "No body", strlen("No body"));
        body[strlen("No body") + 1] = '\0';
    }
    
    printf("%s\n", body);
    free(body);
    exit(0);
}