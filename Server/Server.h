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

#define MAX_REQUEST_SIZE 35000

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

struct ClientSocketDetails 
{
    int socket;
    char method[12];
    char route[100];
    char siteDirectory[1024];
    char request_buffer[MAX_REQUEST_SIZE];
};

void launch (struct Server *server);
void reciveAndSendDataOnSeparateThread(struct ClientSocketDetails *client);
struct Server server_constructor(int domain, int service, int protocol, char* interface, int port, int backlog, char *websiteDirectoryPath, void (*launch)(struct Server *server));
bool establishingFilePathAndDataType(char *filePath, char *method, char *route, char *MIMEtype, size_t path_size);
void createResponse(char *fullPath, char *MIMEtype, char **response_buffer);
void writingAndSendingAResoponse(int socket, char* filePath, char* MIMEtype);
bool sendResponse(struct ClientSocketDetails *client);

#endif