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

#define MAX_REQUEST_SIZE 64000

struct Server 
{
    int domain;
    int service;
    int protocol;
    char* ip;
    int port;
    int backlog;
    int socket;
    char *websiteDirectoryPath;

    struct sockaddr_in address;
};

struct Response 
{
    char *string;
    size_t size;
};

struct ClientSocketDetails 
{
    int socket;
    char method[16];
    char route[256];
    char siteDirectory[256];
    char request_buffer[MAX_REQUEST_SIZE];
};

void reciveAndSendDataOnSeparateThread(int client_socket, char *webSiteDirPath);

#endif