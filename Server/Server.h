#ifndef Server_h
#define Server_h

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

struct Server 
{
    int domain;
    int service;
    int protocol;
    char* interface;
    int port;
    int backlog;
    int socket;
    char *websiteDirectoryPath;

    struct sockaddr_in address;
    void (*launch)(struct Server *server);
};

struct Response 
{
    char *string;
    size_t size;
};

void launch (struct Server *server);
struct Server server_constructor(int domain, int service, int protocol, char* interface, int port, int backlog, char *websiteDirectoryPath, void (*launch)(struct Server *server));
bool establishingFilePathAndDataType(char *filePath, char *method, char *route, char *MIMEtype, size_t path_size);
void createResponse(char *fullPath, char *MIMEtype, char **response_buffer);

#endif