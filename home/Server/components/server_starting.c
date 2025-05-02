#include "../headers/server_structures.h"
#include "../headers/response_header.h"

struct serverStructure createServer(int protocol, int backlog) 
{
    struct serverStructure server;
    server.address.sin_family = AF_INET;
    server.address.sin_port = htons(PORT);
    server.address.sin_addr.s_addr = htonl(INADDR_ANY);
    server.socket = socket(PF_INET, SOCK_STREAM, protocol);

    if (server.socket < 0)
    {
        perror("(Log) Failed to connect socket...\n");
        exit(-1);
    }

    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
    {
        perror("(Log) Failed to bind socket...\n");
        exit(-1);
    }

    if (listen(server.socket, backlog) < 0)
    {
        perror("(Log) Failed to start listening...\n");
        exit(-1);
    }
    
    char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
    int error = getnameinfo((struct sockaddr *)&server.address, sizeof(server.address), hostBuffer, sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);
    
    if (error != 0) {
        printf("(Log) Error: %s\n", gai_strerror(error));
        exit(-1);
    }

    printf("\n(Log) Server is listening on http://%s:%d/\n\n", hostBuffer ,ntohs(server.address.sin_port));
    return server;
}

int acceptingRequest(struct serverStructure server)
{
    int socket = 0;
    int address_length = sizeof(server.address);
    if ((socket = accept(server.socket, (struct sockaddr *)&server.address, (socklen_t *)&address_length)) < 0)
    {
        close(socket);
        return -1;
    }

    printf("(Log) Client Socket: %d\n", socket);
    return socket;
}

int listeningForRequest(struct serverStructure server, char* dir, int fd) 
{
    int clientSocket = 0;
    
    // Waiting for a client request
    while(true)
    {
        dprintf(fd, "\n===== WAITING =====\n");
        if ((clientSocket = acceptingRequest(server)) == -1) {
            fprintf(stderr, "Failed to accept client request...\n");
            return -1;
        }

        if (constructResponseOnASeparateThread(clientSocket, dir, fd) == -1) {
            fprintf(stderr, "Failed to construct a response...\n");
            return -1;
        }
    }

    return 0;
}