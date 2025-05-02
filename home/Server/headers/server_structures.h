#ifndef server_structures_h
#define server_structures_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <curl/curl.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_REQUEST_SIZE 4096
#define PORT 8080

struct serverStructure 
{
    int socket;
    struct sockaddr_in address;
};

// Not needed for now
struct Response 
{
    char *string;
    size_t size;
};

struct Client 
{
    int socket;
    int fd;
    char method[16];
    char route[256];
    char dir[256];
    char requestBuffer[MAX_REQUEST_SIZE];
    // char* body;
};

#endif