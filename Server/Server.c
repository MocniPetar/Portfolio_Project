#include "Server.h"
#include <fcntl.h>
#include <netdb.h>
#include <curl/curl.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <Kernel/string.h>

#define PORT 8080

int server_socket = 0;

struct Server server_constructor(int domain, int service, int protocol, char* ip, 
    int port, int backlog, char* websiteDirectoryPath)
{
    struct Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.ip = ip;
    server.port = port;
    server.backlog = backlog;
    server.websiteDirectoryPath = websiteDirectoryPath;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    inet_pton(server.domain, ip, &server.address.sin_addr.s_addr);

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

    snprintf(response_buffer, strlen(response_buffer), "%s{\n\t\"hex\": \"%s\"\n}", response_heading, color_value);

    // ============================

    curl_easy_cleanup(curl);
    free(response.string);
}

bool establishingFilePathAndDataType(char *filePath, char *method, char *route, char *MIMEtype, size_t dir_path) 
{   
    if (strcmp(method, "GET") != 0)
    {
        printf("%s is an unacceptable method...\n", method);
        return false;
    }

    if (route[strlen(route) - 1] == '/') {
        snprintf(MIMEtype, 16, ".html");
        strncpy(filePath + dir_path, "/index.html", strlen(filePath) - 1);
        filePath[strlen(filePath) + 1] = '\0';
        printf("\n%s\n", filePath);
        printf("%s\n", MIMEtype);
    

        return true;
    }

    int start_of_mime_type = 0;
    int route_length = strlen(route);

    for (int i = 0; i < route_length; i++) {
        if (route[i] == '.') {
            start_of_mime_type = i;
            break;
        }
    }
    
    if (start_of_mime_type > 0) 
    {
        snprintf(MIMEtype, 16, "%s", &route[start_of_mime_type]);

        if (strcmp(MIMEtype, ".html") == 0) 
        {
            strncpy(filePath + dir_path, "/index.html", strlen(filePath) - 1);
            filePath[strlen(filePath) + 1] = '\0';

        }
        else if (strcmp(MIMEtype, ".css") == 0) 
        {
            if (strcmp(route, "/not_found.css") == 0) { 
                snprintf(filePath, (strlen("../WebSite/not_found/not_found.css") + 1), "../WebSite/not_found/not_found.css");
            }
            else { 
                strncpy(filePath + dir_path, "/style.css", strlen(filePath) - 1);
                filePath[strlen(filePath) + 1] = '\0';

            }
        }
        else if (strcmp(MIMEtype, ".js") == 0) 
        {
            strncpy(filePath + dir_path, "/script.js", strlen(filePath) - 1);
            filePath[strlen(filePath) + 1] = '\0';

        }
    }
    // Add an else if here when connecting the frontend with the backend becouse the server can catch requests to the backend
    // and if they are defined in the backend you do not want ot return not_found
    else 
    {
        snprintf(MIMEtype, 16, ".html");
        snprintf(filePath, (strlen("../WebSite/not_found/index.html") + 1), "../WebSite/not_found/index.html");
    }
    printf("\n%s\n", filePath);
    printf("%s\n", MIMEtype);

    return true;
}

void createResponse(char *fullPath, char *MIMEtype, char **response_buffer) 
{

    // Check if backend end point exists from the fullPath (using if-else statements)
    // Call backend function and process the data if exists
    // Return backend response from here

    // If not a backend call continue with sending site data

    FILE *file = fopen(fullPath, "r");
    char *file_buffer = NULL;

    //printf("Sending file at path: %s\n", fullPath);
    if (file == NULL)
    {
        size_t problem_response_size = strlen("HTTP/1.1 500 Problem\r\n\r\n");
        *response_buffer = malloc(problem_response_size+1);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            exit(EXIT_FAILURE);
        }

        strncpy(*response_buffer, "HTTP/1.1 500 Problem\r\n\r\n", problem_response_size);
        (*response_buffer)[problem_response_size] = '\0';
        return;
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

        size_t response_setup_size = strlen(response_setup_string);
        
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        file_buffer = (char *)malloc(file_size + 1);
        if (file_buffer == NULL)
        {
            fprintf(stderr, "Failed to allocate memory...\n");
            fclose(file);
            exit(1);
        }

        fread(file_buffer, 1, file_size, file);
        file_buffer[file_size] = '\0';
        fclose(file);

        size_t new_response_buffer_size = response_setup_size + file_size + 1;
        *response_buffer = (char *)malloc(new_response_buffer_size);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            free(file_buffer);
            exit(EXIT_FAILURE);
        }
        strcpy(*response_buffer, response_setup_string);
        strcat(*response_buffer, file_buffer);

        free(file_buffer);
    }
}

void writingAndSendingAResoponse(int socket, char* filePath, char* MIMEtype) 
{
    char *response_buffer = NULL;
    createResponse(filePath, MIMEtype, &response_buffer);
    
    if (response_buffer == NULL) {
        fprintf(stderr, "Failed to create response...\n");
        return;
    }

    if (send(socket, response_buffer, strlen(response_buffer), 0) < 0)
    {
        fprintf(stderr, "Failed to send response...\n");
        exit(1);
    }

    free(response_buffer);
}

bool sendResponse(int socket, char* method, char* route, char* siteDirectory)
{
    char filePath[1024];
    char MIMEtype[16];
    snprintf(filePath, sizeof(filePath), "%s", siteDirectory);
    size_t dir_length = strlen(siteDirectory);

    if(!establishingFilePathAndDataType(filePath, method, route, MIMEtype, dir_length)) 
    {
        printf("Failed to establish file path and data type...\n");
        return false;
    }
    writingAndSendingAResoponse(socket, filePath, MIMEtype);
    return true;
}

void* reciveAndSendData(void* arg) 
{
    struct ClientSocketDetails* client = (struct ClientSocketDetails*) arg;

    if (recv(client->socket, client->request_buffer, MAX_REQUEST_SIZE, 0) < 0)
    {
        fprintf(stderr, "Failed to read request_buffer to socket...\n");
        close(client->socket);
        free(client);
        pthread_exit(NULL);
    }
    //printf("%s\n", client->request_buffer);
    sscanf(client->request_buffer, "%s %s", client->method, client->route);
    client->method[strlen(client->method) + 1] = '\0';
    client->route[strlen(client->route) + 1] = '\0';

    printf("Request accepted, sending response...\n");
    if(!sendResponse(client->socket, client->method, client->route, client->siteDirectory)) {
        printf("Failed to send response to client...\n");
    }
    close(client->socket);
    free(client);
    pthread_exit(NULL);
}

void reciveAndSendDataOnSeparateThread(int client_socket, char *webSiteDirPath) 
{
    pthread_t id;
    struct ClientSocketDetails* client = (struct ClientSocketDetails*)malloc(sizeof(struct ClientSocketDetails));

    if (client == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return;
    }

    client->socket = client_socket;
    strcpy(client->siteDirectory, webSiteDirPath);

    int ret = pthread_create(&id, NULL, reciveAndSendData, client);
    if (ret != 0) {
        fprintf(stderr, "Failed to create thread. Error code: %d\n", ret);
        free(client);
        return;
    }
    pthread_join(id, NULL);
}

void handle_sigint(int sig) {
    printf("\nExiting program...\n");
    close(server_socket);
    exit(0);
}

int main (int argc, char **argv) 
{
    if (signal(SIGINT, handle_sigint) == SIG_ERR) 
    {
        perror("Failed to set signal handler");
        exit(1);
    }

    if (argc < 2) 
    {
        printf("Missing project build directory...\n");
        exit(1);
    }

    DIR *dir = opendir(argv[1]);
    if (dir) 
    {
        closedir(dir);
    } else 
    {
        printf("Failed to locate/open project build directory...\n");
        exit(1);
    }

    printf("\nSuccessfully located the project build directory...\n");

    // This is executed only once to create the server socket
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, "192.168.0.153", PORT, 10, argv[1]);
    server_socket = server.socket;

    int address_length = sizeof(server.address);
    int client_socket = 0;
    
    // Waiting for a client request
    while(true)
    {
        printf("\n===== WAITING =====\n");
        printf("Waiting for client request...\n");

        // At this part the program stops and listens for a connection
        if ((client_socket = accept(server.socket, (struct sockaddr *)&server.address, (socklen_t *)&address_length)) < 0)
        {
            fprintf(stderr, "Failed to accept client...\n");
            break;
        }

        // Create multiple threads
        reciveAndSendDataOnSeparateThread(client_socket, server.websiteDirectoryPath);
    }
    close(server.socket);
    exit(0);
}