#include "Server.h"
#include <signal.h>

void launch (struct Server *server) 
{
    int address_length = sizeof(server->address);
    struct ClientSocketDetails client;
    strcpy(client.siteDirectory, server->websiteDirectoryPath);

    while(true)
    {
        printf("\n===== WAITING =====\n");
        printf("Waiting for client request...\n");

        // At this part the program stops and listens for a connection
        if ((client.socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length)) < 0)
        {
            fprintf(stderr, "Failed to accept client...\n");
            break;
        }

        // Create multiple threads
        reciveAndSendDataOnSeparateThread(&client);
    }
    close(server->socket);
}