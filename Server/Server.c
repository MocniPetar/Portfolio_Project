#include "Server.h"
#include <fcntl.h>
#include <netdb.h>
#include <curl/curl.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>

#define PORT 8080

void* reciveAndSendData(void *);

struct Server server_constructor(int domain, int service, int protocol, char* interface, 
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
    inet_pton(server.domain, interface, &server.address.sin_addr.s_addr);

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

bool establishingFilePathAndDataType(char *filePath, char *method, char *route, char *MIMEtype, size_t path_size) 
{   
    if (strcmp(method, "GET") != 0)
    {
        printf("%s is an unacceptable method...\n", method);
        return false;
    }

    if (route[strlen(route) - 1] == '/') {
        strcpy(MIMEtype, ".html");
        strcpy(filePath + path_size, "/index.html");
        return true;
    }

    int start_of_mime_type = 0;
    while(start_of_mime_type < strlen(route)) {
        if (route[start_of_mime_type] == '.') {
            break;
        }
        start_of_mime_type++;
    }
    strcpy(MIMEtype, &route[start_of_mime_type]);
    if (strcmp(MIMEtype, ".html") == 0) 
    {
        strcpy(filePath + path_size, route);
    }
    else if (strcmp(MIMEtype, ".css") == 0) 
    {
        if (strcmp(route, "/not_found.css") == 0) { strcpy(filePath, "../WebSite/not_found/not_found.css"); }
        else { strcpy(filePath + path_size, route); }
    }
    else if (strcmp(MIMEtype, ".js") == 0) 
    {
        strcpy(filePath + path_size, route);
    }
    // Add an else if here when connecting the frontend with the backend becouse the server can catch requests to the backend
    // and if they are defined in the backend you do not want ot return not_found
    else 
    {
        strcpy(filePath, "../WebSite/not_found/index.html");
        strcpy(MIMEtype, ".html");
    }

    return true;
}

void createResponse(char *fullPath, char *MIMEtype, char **response_buffer) 
{

    // Check if backend end point exists from the fullPath (using if-else statements)
    // Call backend function and process the data if exists
    // Return backend response from here

    // If not a backend call continue with sending site data

    FILE *file = fopen(fullPath, "r");
    printf("Sendin file at path: %s\n", fullPath);
    if (file == NULL)
    {
        strcpy(*response_buffer, "HTTP/1.1 500 Problem\r\n\r\n");
        return;
    }

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
    response_buffer[strlen(*response_buffer)] = 0;
    free(file_buffer);
    fclose(file);
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

bool sendResponse(struct ClientSocketDetails *client)
{
    char filePath[1024];
    char MIMEtype[12];
    strcpy(filePath, client->siteDirectory);
    if(!establishingFilePathAndDataType(filePath, client->method, client->route, MIMEtype, strlen(client->siteDirectory))) 
    {
        printf("Failed to establish file path and data type...\n");
        close(client->socket);
        return false;
    }
    else {
        writingAndSendingAResoponse(client->socket, filePath, MIMEtype);
    }
    return true;
}

void reciveAndSendDataOnSeparateThread(struct ClientSocketDetails *client) 
{
    pthread_t id;
    pthread_create(&id, NULL, reciveAndSendData, client);
}

void* reciveAndSendData(void* arg) 
{
    struct ClientSocketDetails* client = (struct ClientSocketDetails*) arg;
    if (recv(client->socket, client->request_buffer, MAX_REQUEST_SIZE, 0) < 0)
    {
        fprintf(stderr, "Failed to read request_buffer to socket...\n");
        pthread_exit(NULL);
    }
    printf("%s\n", client->request_buffer);
    sscanf(client->request_buffer, "%s %s", client->method, client->route);

    printf("Request accepted, sending response...\n");
    if(!sendResponse(client)) {
        printf("Failed to send response to client...\n");
    }
    close(client->socket);
    return NULL;
}

void handle_sigint(int sig) {
    printf("Exiting program...\n");
    exit(0);
}

int main (int argc, char **argv) 
{
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("Failed to set signal handler");
        exit(0);
    }
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

    // This is executed only once to create the server socket
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, "127.0.0.1", PORT, 10, argv[1], launch);

    // This part containes a while loop and it awaits for a client to connect
    server.launch(&server);
}