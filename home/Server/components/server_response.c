#include "../headers/server_structures.h"
#include "../headers/starting_header.h"

pthread_mutex_t lock; 

int parseRequest(struct Client* client) 
{
    ssize_t receivedBytes = recv(client->socket, client->requestBuffer, MAX_REQUEST_SIZE - 1, 0);
    if (receivedBytes <= 0) {
        fprintf(stderr, "(Log) Failed to read request_buffer to socket...\n");
        dprintf(client->fd, "(Log) Failed to read request_buffer to socket...\n");
        return -1;
    }
    client->requestBuffer[receivedBytes] = '\0';

    dprintf(client->fd, "Request:\n%s\n", client->requestBuffer);
    sscanf(client->requestBuffer, "%s %s", client->method, client->route);
    client->method[strlen(client->method) + 1] = '\0';
    client->route[strlen(client->route) + 1] = '\0';
    
    ssize_t dirLength = strlen(client->dir);
    ssize_t routeLength = strlen(client->route);

    char* temp_route = (char *)malloc(routeLength + dirLength + 1);
    if (temp_route == NULL) {
        printf("(Log) Failed to allocate memory using malloc for temp_route...\n");
        return -1;
    }
    strcpy(temp_route, client->dir);
    strcat(temp_route, client->route);
    temp_route[routeLength + dirLength] = '\0';
    strcpy(client->route, temp_route);

    free(temp_route);
    return 0;
}

int getReguestedRoute(char *filePath, char *method, char *MIMEtype) 
{
    if (strcmp(method, "GET") != 0)
    {
        printf("(Log) %s is an unacceptable method...\n", method);
        return -1;
    }

    ssize_t filePathLength = strlen(filePath);
    for (int i = 2; i < (int)filePathLength; i++) {
        if (filePath[i] == '.') { 
            snprintf(MIMEtype, 16, "%s", &filePath[i]);
            break;
        }
    }
    if(strlen(MIMEtype) < 1)
    {
        if (filePath[filePathLength-1] == '/')
            strcat(filePath, "index.html");
        else
            strcat(filePath, "/index.html");
        
        snprintf(MIMEtype, 16, ".html");
        filePath[strlen(filePath) + 1] = '\0';
    }
    
    if (access(filePath, F_OK) != 0) {
        printf("(Log) File does not exist. Re-routing client to not found...\n\n");
        snprintf(filePath, 256, "../WebSite/src/pages/not_found/index.html");
        snprintf(MIMEtype, 16, ".html");
    }

    return 0;
}

int writingResponse(char *fullPath, char *MIMEtype, char **response_buffer, int fd) 
{
    // Check if backend end point exists from the fullPath (using if-else statements)
    // Call backend function and process the data if exists
    // Return backend response from here

    // If not a backend call continue with sending site data

    FILE *file = fopen(fullPath, "r");
    char *file_buffer = NULL;

    if (file == NULL)
    {
        size_t problem_response_size = strlen("HTTP/1.1 500 Problem\r\n\r\n");
        *response_buffer = malloc(problem_response_size+1);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            return -1;
        }

        strncpy(*response_buffer, "HTTP/1.1 500 Problem\r\n\r\n", problem_response_size);
        (*response_buffer)[problem_response_size] = '\0';
    }
    else 
    {
        char response_setup_string[256];
        if (strcmp(MIMEtype, ".html") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".css") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: text/css; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".js") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: text/javascript; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".svg") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: image/svg+xml; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }

        size_t response_setup_size = strlen(response_setup_string);
        
        fseek(file, 0, SEEK_END);
        ssize_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        file_buffer = (char *)malloc(file_size + 1);
        if (file_buffer == NULL)
        {
            fprintf(stderr, "(Log) Failed to allocate memory...\n");
            dprintf(fd, "(Log) Failed to allocate memory...\n");
            fclose(file);
            return -1;
        }

        if(fread(file_buffer, 1, file_size, file) <= 0) {
            fprintf(stderr, "(Log) Failed to read data from file...\n");
            dprintf(fd, "(Log) Failed to read data from file...\n");
            free(file_buffer);
            fclose(file);
            return -1;
        }
        file_buffer[file_size] = '\0';

        *response_buffer = (char *)malloc(response_setup_size + file_size + 1);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            fclose(file);
            free(file_buffer);
            return -1;
        }
        strcpy(*response_buffer, response_setup_string);
        strcat(*response_buffer, file_buffer);

        fclose(file);
        free(file_buffer);
    }

    return 0;
}

int sendingResponse(int clientSocket, char* response_buffer, int fd)
{
    if (send(clientSocket, response_buffer, strlen(response_buffer), 0) < 0)
    {
        fprintf(stderr, "(Log) Failed to send response...\n");
        dprintf(fd, "(Log) Failed to send response...\n");
        return -1;
    }

    return 0;
}

void* constructResponse(void* arg) 
{
    pthread_mutex_lock(&lock);
    struct Client client = *((struct Client*)arg);
    free(arg);

    // Take the request and process it
    if (parseRequest(&client) == -1) {
        printf("(Log) Error occured while parsing the request...\n");
        dprintf(client.fd, "(Log) Error occured while parsing the request...\n");
        close(client.socket);
        pthread_mutex_unlock(&lock); 
        return NULL;
    }

    char filePath[1024];
    char MIMEtype[16];
    snprintf(filePath, sizeof(filePath), "%s", client.route);

    // Get the requested route
    if (getReguestedRoute(filePath, client.method, MIMEtype) == -1) {
        printf("(Log) Error occured while creating the route...\n");
        dprintf(client.fd, "(Log) Error occured while creating the route...\n");
        close(client.socket);
        pthread_mutex_unlock(&lock); 
        return NULL;
    }

    // This is what I am testing at the moment
    // printf("Socket: %d\nFd: %d\nMethod: %s\nRoute: %s\nDirectory: %s\nFile path: %s\n", 
    //     client.socket, client.fd, client.method, client.route, client.dir, filePath);

    char* responseBuffer = NULL;
    if (writingResponse(filePath, MIMEtype, &responseBuffer, client.fd) == -1) {
        if (responseBuffer != NULL) { free(responseBuffer); }
        close(client.socket);
        pthread_mutex_unlock(&lock); 
        return NULL;
    }

    if (responseBuffer == NULL) {
        printf("(Log) ERROR: Response buffer is NULL!\n");
        close(client.socket);
        pthread_mutex_unlock(&lock); 
        return NULL;
    }

    // Send a response to the client
    if (sendingResponse(client.socket, responseBuffer, client.fd) == -1) {
        printf("(Log) Error occured while sending the response...\n");
        dprintf(client.fd, "(Log) Error occured while sending the response...\n");
    }

    printf("(Log) Sent page data to client\n");

    close(client.socket);
    free(responseBuffer);
    pthread_mutex_unlock(&lock); 
    return NULL;
}

// ----------------------------- //
// ADD mutex locking for threads so buffer overflows are a thing of the past
// ----------------------------- //

int constructResponseOnASeparateThread(int clientSocket, char* dir, int fd) 
{
    struct Client* client = (struct Client*)malloc(sizeof(struct Client));
    if (client == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return -1;
    }
    
    client->socket = clientSocket;
    client->fd = fd;
    strcpy(client->dir, dir);

    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("(Log) mutex init has failed\n"); 
        return -1; 
    }
    
    // Create a thread
    pthread_t thread;
    int result = pthread_create(&thread, NULL, &constructResponse, client);
    if (result != 0) {
        fprintf(stderr, "Failed to create thread. Error code: %ld\n", thread);
        dprintf(fd, "Failed to create thread. Error code: %ld\n", thread);
        pthread_mutex_destroy(&lock);
        return -1;
    }

    pthread_join(thread, NULL);
    pthread_mutex_destroy(&lock);
    return 0;
}
