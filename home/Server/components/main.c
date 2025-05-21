#include "../headers/server_structures.h"
#include "../headers/starting_header.h"
#include "../headers/important_header.h"

#include <signal.h>

int server_socket = 0;
int fd = 0;

void handle_sigint(int sig) {
    printf("\n(Log) Exiting program\n");
    exit(0);
}

void cleanup() {
    dprintf(fd, "(Log) End of session\n");
    close(fd);
    close(server_socket);
    printf("(Log) Closing file descriptors and sockets\n");
}

int main(int argc, char** argv) 
{
    atexit(cleanup);

    if (argc < 2)
    {
        printf("(Log) Missing project build directory\n");
        exit(1);
    }

    if (signal(SIGINT, handle_sigint) == SIG_ERR) 
    {
        perror("(Log) Failed to set signal handler");
        exit(1);
    }

    DIR *dir = opendir(argv[1]);
    if (dir) {
        printf("\n(Log) Successfully located the project build directory\n");
        printf("(Log) Reading the contents of this directory and storing page specific data in a list\n");
        check(readContentsOfDirectory(dir, argv[1]), "(Log) Problem occured in function createListOfPages\n", 0);
        closedir(dir);
    } else {
        printf("(Log) Failed to locate/open project build directory\n");
        exit(1);
    }
    check((fd = open("./logs/output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)), "(Log) Error occured while listening for requests", 0);


    // Create server
    // struct serverStructure server = createServer(0, 10);
    // server_socket = server.socket;

    // listeningForRequest(server, argv[1], fd);
    exit(0);
}