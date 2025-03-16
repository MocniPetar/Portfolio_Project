#include "Server.h"
#include <signal.h>
#define MAX_REQUEST_SIZE 35000

void handle_sigint(int sig) 
{
    exit(0);
}

void writingAndSendingAResoponse(int socket, char* filePath, char* MIMEtype) 
{
    char *response_buffer = (char *)malloc(1024);
    if (!response_buffer)
    {
        fprintf(stderr, "Failed to allocate memory for response_buffer...");
        exit(1);
    }
    createResponse(filePath, MIMEtype, &response_buffer);
    
    if (strlen(response_buffer) <= 5 && strcmp(response_buffer, "error")) {
        send(socket, "HTTP/1.1 500 Problem\r\n\r\n", (size_t)strlen("HTTP/1.1 500 Problem\r\n\r\n"), 0);
    }
    else {
        if (write(socket, response_buffer, strlen(response_buffer)) < 0)
        {
            fprintf(stderr, "Failed to send response...\n");
            exit(1);
        }
    }

    if (response_buffer != NULL)
        free(response_buffer);
}

void launch (struct Server *server) 
{
    char request_buffer[MAX_REQUEST_SIZE], method[10], route[100];
    int address_length = sizeof(server->address);
    int client_socket = 0;

    while(true)
    {
        printf("\n===== WAITING =====\n");
        printf("Waiting for client request...\n");

        // At this part the program stops and listens for a connection
        if ((client_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length)) < 0)
        {
            fprintf(stderr, "Failed to accept client...\n");
            break;
        }
        if (recv(client_socket, request_buffer, MAX_REQUEST_SIZE, 0) < 0)
        {
            fprintf(stderr, "Failed to read request_buffer to socket...\n");
            break;
        }

        shutdown(client_socket, SHUT_RD);

        printf("%s\n", request_buffer);
        sscanf(request_buffer, "%s %s", method, route);

        printf("Request accepted, sending response...\n");

        char filePath[1024];
        char MIMEtype[12];
        strcpy(filePath, server->websiteDirectoryPath);

        if(!establishingFilePathAndDataType(filePath, method, route, MIMEtype, strlen(server->websiteDirectoryPath))) 
        {
            printf("Failed to establish file path and data type...\n");
            close(client_socket);
            break;
        }
        else {
            writingAndSendingAResoponse(client_socket, filePath, MIMEtype);
        }

        close(client_socket);
    }
    close(server->socket);
}