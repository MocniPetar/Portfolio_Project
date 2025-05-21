#include "../headers/server_structures.h"
#include "../headers/starting_header.h"
#include "../headers/important_header.h"

int parseRequest(struct Client*);
int getReguestedRoute(char*, char*, char*);
int writingResponse(char *, char *, char **, int);
int sendingResponse(int, char*, int);
void* constructResponse(void*);

int parseRequest(struct Client* client) 
{
    char temp_buffer[MAX_REQUEST_SIZE + 1];
    ssize_t receivedBytes = 0;
    size_t lastRecievedSize = 0;
    
    char* req_buffer = (char *)malloc((MAX_REQUEST_SIZE + 1) * sizeof(char));
    if (req_buffer == NULL) {
        dprintf(client->fd, "Failed to allocate memmory for req_buffer\n");
        return -1;
    }

    memset(temp_buffer, 0, MAX_REQUEST_SIZE);
    memset(req_buffer, 0, MAX_REQUEST_SIZE);

    while ((receivedBytes = recv(client->socket, temp_buffer, MAX_REQUEST_SIZE - 1, 0)) > 0) 
    {   
        if (lastRecievedSize > strlen(req_buffer)) {
            char *new_req_buffer = realloc(req_buffer, (lastRecievedSize + 1) * sizeof(char));
            if (new_req_buffer == NULL) {
                dprintf(client->fd, "Failed to allocate memmory for new_req_buffer\n");
                free(req_buffer);
                return -1;
            }
            strcat(new_req_buffer, temp_buffer);
            req_buffer = new_req_buffer;
        } else {
            strcat(req_buffer, temp_buffer);
        }

        if (strstr(temp_buffer, "\r\n\r\n") != NULL) { break; }

        lastRecievedSize += receivedBytes;
        memset(temp_buffer, 0, MAX_REQUEST_SIZE);
    }

    dprintf(client->fd, "Request:\n%s\n", req_buffer);
    sscanf(req_buffer, "%s %s", client->method, client->route);
    free(req_buffer);

    printf("(Log) Requested route: %s\n", client->route);

    client->method[strlen(client->method) + 1] = '\0';
    client->route[strlen(client->route) + 1] = '\0';
    
    ssize_t dirLength = strlen(client->dir);
    ssize_t routeLength = strlen(client->route);

    char* temp_route = (char *)malloc(routeLength + dirLength + 1);
    if (temp_route == NULL) {
        printf("(Log) Failed to allocate memory using malloc for temp_route...\n");
        return -1;
    }
    memset(temp_route, 0, routeLength + dirLength + 1);

    snprintf(temp_route, routeLength + dirLength, "%s", client->dir);
    strcat(temp_route, client->route);
    snprintf(client->route, sizeof(client->route), "%s", temp_route);

    free(temp_route);
    return 0;
}

int getReguestedRoute(char *filePath, char *method, char *MIMEtype) 
{
    // This server only accepts GET requests

    if (strcmp(method, "GET") != 0)
    {
        printf("(Log) %s is an unacceptable method\n", method);
        return -1;
    }

    ssize_t filePathLength = strlen(filePath);
    // Skip the first two dots from ../WebSite/dist/browser/{filePath}
    for (int i = 2; i < (int)filePathLength; i++) {
        if (filePath[i] == '.') { 
            snprintf(MIMEtype, 16, "%s", &filePath[i]);
        }
    }
    
    // If MIMEtype is less then one then the client requetsed a page
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
    FILE *file = fopen(fullPath, "r");
    char *file_buffer = NULL;

    if (file == NULL)
    {
        size_t problem_response_size = strlen("HTTP/1.1 500 Problem\r\n\r\n") + 1;
        *response_buffer = (char *)malloc((problem_response_size)*sizeof(char));
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            return -1;
        }
        memset(*response_buffer, 0, problem_response_size);
        snprintf(*response_buffer, problem_response_size, "HTTP/1.1 500 Problem\r\n\r\n");
    }
    else 
    {
        char response_setup_string[256];
        memset(response_setup_string, 0, 256);

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
        else if (strcmp(MIMEtype, ".json") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".ico") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".png") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: image/png; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
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
        memset(file_buffer, 0, file_size + 1);

        if(fread(file_buffer, 1, file_size, file) <= 0) {
            fprintf(stderr, "(Log) Failed to read data from file...\n");
            dprintf(fd, "(Log) Failed to read data from file...\n");
            free(file_buffer);
            fclose(file);
            return -1;
        }

        *response_buffer = (char *)malloc(response_setup_size + file_size + 1);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            fclose(file);
            free(file_buffer);
            return -1;
        }
        memset(*response_buffer, 0, response_setup_size + file_size + 1);

        strncpy(*response_buffer, response_setup_string, response_setup_size + file_size);
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
        fprintf(stderr, "(Log) Failed to send response\n");
        dprintf(fd, "(Log) Failed to send response\n");
        return -1;
    }

    return 0;
}

void* constructResponse(void* arg) 
{
    struct Client client = *((struct Client*)arg);
    free(arg);

    // Take the request and process it
    if (parseRequest(&client) == -1) {
        printf("(Log) Error occured while parsing the request. Check the log for more details\n");
        close(client.socket);
        return NULL;
    }

    char filePath[1024];
    char MIMEtype[16];
    memset(filePath, 0, 1024);
    memset(MIMEtype, 0, 16);

    snprintf(filePath, sizeof(filePath), "%s", client.route);

    // Get the requested route
    if (getReguestedRoute(filePath, client.method, MIMEtype) == -1) {
        printf("(Log) Error occured while creating the route. Check the log for more details\n");
        close(client.socket);
        return NULL;
    }

    // Create the response
    char* responseBuffer = NULL;
    if (writingResponse(filePath, MIMEtype, &responseBuffer, client.fd) == -1) {
        if (responseBuffer != NULL) { free(responseBuffer); }
        close(client.socket);
        return NULL;
    }

    if (responseBuffer == NULL) {
        printf("(Log) ERROR: Response buffer is NULL!\n");
        close(client.socket);
        return NULL;
    }

    // Send the response to the client
    if (sendingResponse(client.socket, responseBuffer, client.fd) == -1) {
        printf("(Log) Error occured while sending the response. Check the log for more details\n");
    }

    close(client.socket);
    free(responseBuffer);
    return NULL;
}