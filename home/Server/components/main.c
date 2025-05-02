#include "../headers/server_structures.h"
#include "../headers/starting_header.h"
#include <signal.h>

int server_socket = 0;
int fd = 0;

void handle_sigint(int sig) {
    printf("\nExiting program...\n");
    exit(0);
}

void cleanup() {
    dprintf(fd, "End of session...\n");
    close(fd);
    close(server_socket);
    printf("Closing file descriptors and sockets...\n");
}

int main(int argc, char** argv) 
{
    atexit(cleanup);

    if (argc < 2)
    {
        printf("Missing project build directory...\n");
        exit(-1);
    }

    if (signal(SIGINT, handle_sigint) == SIG_ERR) 
    {
        perror("Failed to set signal handler");
        exit(-1);
    }

    DIR *dir = opendir(argv[1]);
    if (dir) {
        closedir(dir);
    } else {
        printf("Failed to locate/open project build directory...\n");
        exit(-1);
    }

    printf("\nSuccessfully located the project build directory...\n");

    if ((fd = open("./logs/output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        fprintf(stderr, "Cannot open log file...\n");
        exit(-1);
    }

    // Create server
    struct serverStructure server = createServer(0, 10);
    server_socket = server.socket;

    // Start listening for client requests
    if (listeningForRequest(server, argv[1], fd) == -1) {
        printf("(Log) Error occured while listening for requests...\n");
        exit(-1);
    }

    exit(0);
}