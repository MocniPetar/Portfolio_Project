#include "Server.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>

struct Server server_constructor(int domain, int service, int protocol, u_long interface, 
    int port, int backlog, char* websiteDirectoryPath, void(*launch)(struct Server *server))
{
    struct Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;
    server.websiteDirectoryPath = websiteDirectoryPath;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(server.domain, server.service, server.protocol);
    if (server.socket < 0)
    {
        perror("Failed to connect socket...\n");
        exit(1);
    }

    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
    {
        perror("Failed to bind socket...\n");
        exit(1);
    }

    if (listen(server.socket, server.backlog) < 0)
    {
        perror("Failed to start listening...\n");
        exit(1);
    }
    
    char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
    int error = getnameinfo((struct sockaddr *)&server.address, sizeof(server.address), hostBuffer, sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);
    
    if (error != 0) {
        printf("Error: %s\n", gai_strerror(error));
        exit(1);
    }
    printf("\nServer is listening on http://%s:%d/\n\n", hostBuffer, server.port);
    
    server.launch = launch;
    return server;
}

size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata) 
{
    size_t real_size = size * nmemb;
    struct Response *response = (struct Response *) userdata;
    char *ptr = realloc(response->string, response->size + real_size + 1);
    if (ptr == NULL) {
        return CURL_WRITEFUNC_ERROR;
    }

    response->string = ptr;
    memcpy(&(response->string[response->size]), data, real_size);
    response->size += real_size;
    response->string[response->size] = '\0';

    return real_size;
}

void getRequestedURI(char *fullPath, char *method, char *route, char *MIMEtype, char *directoryPath, size_t path_size) 
{   
    printf("---- In getRequestedURI function ----\n");
    
    if (strcmp(method, "GET") != 0)
    {
        printf("%s is an unacceptable method...\n", method);
        return;
    }
    strcpy(fullPath, directoryPath);
    
    if (route[strlen(route) - 1] == '/') {
        strcpy(MIMEtype, ".html");
        strcpy(fullPath + path_size, "/index.html");
    }
    else {
        int dotIndex = 0;
        for (int i = 0; i < strlen(route); i++) 
        {
            if (route[i] == '.') {  
                dotIndex = i;
                break;
            }
        }

        if (dotIndex > 0)
        {
            strcpy(MIMEtype, &route[dotIndex]);

            if (strcmp(MIMEtype, ".html") == 0) {
                strcpy(fullPath + path_size, route);
            }
            else if (strcmp(MIMEtype, ".css") == 0) {
                if (strcmp(route, "/not_found.css") == 0)
                    strcpy(fullPath, "../WebSite/not_found/not_found.css");
                else
                    strcpy(fullPath + path_size, route);
            }
            else if (strcmp(MIMEtype, ".js") == 0) {
                strcpy(fullPath + path_size, route);
            }
        }
        // Add an else if here when connecting the frontend with the backend becouse the server can catch requests to the backend
        // and if they are defined in the backend you do not want ot return not_found
        else {
            strcpy(fullPath, "../WebSite/not_found/index.html");
            strcpy(MIMEtype, ".html");
        }
    }

    printf("\nPATH: %s\nMIME Type: %s\n", fullPath, MIMEtype);
}

// IN TESTING FAZE
void handle_response(char *response_buffer, char *request_url) 
{
    CURL *curl;
    CURLcode result;
    const char *response_heading =
        "HTTP/1.1 200 OK\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Type: application/json\r\n"
        "\r\n";

    curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "HTTP request failed\n");
        exit(1);
    }

    struct Response response;
    response.string = malloc(1);
    response.size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, request_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
        exit(1);
    }

    // ===== SUBJECT TO CHANGE =====

    char color_value[8];
    strcpy(color_value, &response.string[response.size - 6]);

    sprintf(response_buffer, "%s{\n\t\"hex\": \"%s\"\n}", response_heading, color_value);

    // ============================

    curl_easy_cleanup(curl);
    free(response.string);
}

void handleRequest(char *fullPath, char *MIMEtype, char **response_buffer) 
{
    FILE *file = fopen(fullPath, "r");
    printf("%s\n", fullPath);
    if (file == NULL)
    {
        /*
            If file = NULL then configure the fullPath to be of the not_found
        */

        /*
        FOR LATER OPERATIONS ADD ADDITIONAL IF-ELSE statements to execute certen operations with the backend part of the project
        Like using curl to connect with the backend if the file = NULL

        //handle_response(*response_buffer, "http://localhost:7033/api/Account");
        */
        strcpy(*response_buffer, "HTTP/1.1 500 Problem\r\n\r\n");
    }
    else 
    {
        if (strcmp(MIMEtype, ".html") == 0) {
            sprintf(*response_buffer, "%s", "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".css") == 0) {
            sprintf(*response_buffer, "%s", "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".js") == 0) {
            sprintf(*response_buffer, "%s", "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n\r\n");
        }
        
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *file_buffer = (char *)malloc(file_size);
        if (file_buffer == NULL)
        {
            perror("malloc");
            exit(1);
        }

        size_t bytesRead = fread(file_buffer, 1, file_size, file);
        file_buffer[bytesRead] = '\0';
        size_t file_buffer_size = strlen(file_buffer);

        size_t current_response_size = strlen(*response_buffer);
        if ((current_response_size + file_buffer_size + 1) > (current_response_size + file_buffer_size)) {
            size_t new_response_buffer_size = current_response_size + file_buffer_size + 1;
            char *temp = realloc(*response_buffer, new_response_buffer_size);
            if (temp == NULL)
            {
                fprintf(stderr, "malloc");
                free(file_buffer);
                exit(1);
            }
            *response_buffer = temp;
        }

        strcat(*response_buffer, file_buffer);
        free(file_buffer);
        fclose(file);
    }
}

void launch (struct Server *server) 
{
    char request_buffer[30000], method[10], route[100];
    int address_length = sizeof(server->address);
    int new_socket;

    while(1)
    {
        printf("===== WAITING FOR CONNECTION =====\n");

        if ((new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length)) < 0)
        {
            fprintf(stderr, "Failed to accept client...\n");
            exit(1);
        }
        if (read(new_socket, request_buffer, 30000) < 0)
        {
            fprintf(stderr, "Faile to read request_buffer to socket...\n");
            exit(1);
        }
        
        // Client request
        printf("\n\n Client request:\n%s\n\n", request_buffer);
        sscanf(request_buffer, "%s %s", method, route);

        char fullPath[1024];
        char MIMEtype[12];
        char *response_buffer = (char *)malloc(1024);
        if (!response_buffer)
        {
            fprintf(stderr, "Failed to allocate memory for response_buffer...");
            exit(1);
        }

        getRequestedURI(fullPath, method, route, MIMEtype, server->websiteDirectoryPath, strlen(server->websiteDirectoryPath));
        
       handleRequest(fullPath, MIMEtype, &response_buffer);
        if (strlen(response_buffer) <= 5 && strcmp(response_buffer, "error")) {
            send(new_socket, "HTTP/1.1 500 Problem\r\n\r\n", (size_t)strlen("HTTP/1.1 500 Problem\r\n\r\n"), 0);
        }
        else {
            printf("Sending site data...\n");
            if (write(new_socket, response_buffer, strlen(response_buffer)) < 0)
            {
                fprintf(stderr, "Failed to write buffer to socket...\n");
                exit(1); 
            }
        }

        if (response_buffer != NULL)
            free(response_buffer);

        if(close(new_socket) < 0)
        {
            fprintf(stderr, "Failed to close socket...\n");
            exit(1);
        }
        printf("\n\nSocket is closed...\n\n");
    }
}

int main (int argc, char **argv) 
{
    if (argc < 2) {
        printf("Missing project build directory...\n");
        exit(0);
    }
    DIR *dir = opendir(argv[1]);
    if (dir) {
        closedir(dir);
    } else {
        printf("Failed to locate/open project build directory...\n");
        exit(1);
    }

    printf("\nSuccessfully located the project build directory...\n");

    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_LOOPBACK, 8080, 10, argv[1], launch);
    server.launch(&server);
}