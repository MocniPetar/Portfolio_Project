#include "Server.h"

#define PORT 8080

int server_socket = 0;
int fd = 0;

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
    // This is where the recieved data is going to be parsed

    char color_value[8];
    strcpy(color_value, &response.string[response.size - 6]);

    snprintf(response_buffer, strlen(response_buffer), "%s{\n\t\"hex\": \"%s\"\n}", response_heading, color_value);

    // ============================

    curl_easy_cleanup(curl);
    free(response.string);
}

bool establishingFilePathAndDataType(char *filePath, char *method, char *route, char *MIMEtype, size_t dir_path_size) 
{   
    if (strcmp(method, "GET") != 0)
    {
        printf("%s is an unacceptable method...\n", method);
        return false;
    }

    size_t route_length = strlen(route);
    size_t index_size = strlen("/index.html");
    int index = 0, slashIndex = 0;

    for (int i = 0; i < route_length; i++) 
    {
        if (route[i] == '.') { index = i; break; }
        if (route[i] == '/') { slashIndex = i; }
    }
    if (index > 0)
    {
        snprintf(MIMEtype, 16, "%s", &route[index]);
        if (strcmp(&route[slashIndex], "/not_found.css") == 0) {
            snprintf(filePath, 256, "../WebSite/not_found/not_found.css");
        }
        else if (strcmp(MIMEtype, ".css") == 0) {
            strncpy(filePath + dir_path_size, route, 256 - strlen(filePath) - 1);
            filePath[strlen(filePath) + 1] = '\0';
        }
        else if (strcmp(MIMEtype, ".js") == 0) {
            strncpy(filePath + dir_path_size, route, 256 - strlen(filePath) - 1);
            filePath[strlen(filePath) + 1] = '\0';
        }
    }
    else 
    {   
        snprintf(MIMEtype, 16, ".html");
        if (route[route_length - 1] == '/') {
            strcpy(filePath + dir_path_size, "/index.html");
        }
        else {
            char routeWithIndex[index_size + route_length + 8];
            strcpy(routeWithIndex, route);
            strcpy(routeWithIndex + route_length, "/index.html");
            routeWithIndex[index_size + route_length + 1] = '\0';
            strncpy(filePath + dir_path_size, routeWithIndex, 256 - strlen(filePath) - 1);
        }

        filePath[strlen(filePath) + 1] = '\0';
    }
    
    printf("%s\n", filePath);
    if (access(filePath, F_OK) != 0) {
        printf("%s\n", filePath);
        printf("File does not exist. Routing to not fount...\n");
        snprintf(filePath, 256, "../WebSite/not_found/index.html");
        snprintf(MIMEtype, 16, ".html");
    }

    return true;
}

int createResponse(char *fullPath, char *MIMEtype, char **response_buffer) 
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
            dprintf(fd, "Failed to allocate memory...\n");
            return 0;
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

        size_t response_setup_size = strlen(response_setup_string);
        
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        file_buffer = (char *)malloc(file_size + 1);
        if (file_buffer == NULL)
        {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            fclose(file);
            return 0;
        }

        if(fread(file_buffer, 1, file_size, file) <= 0) {
            fprintf(stderr, "Failed to read data from file...\n");
            dprintf(fd, "Failed to read data from file...\n");
            free(file_buffer);
            fclose(file);
            return 0;
        }

        file_buffer[file_size] = '\0';

        size_t new_response_buffer_size = response_setup_size + file_size + 1;
        *response_buffer = (char *)malloc(new_response_buffer_size);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            fclose(file);
            free(file_buffer);
            return 0;
        }
        strcpy(*response_buffer, response_setup_string);
        strcat(*response_buffer, file_buffer);

        fclose(file);
        free(file_buffer);
    }
    return 1;
}

int writingAndSendingAResoponse(int socket, char* filePath, char* MIMEtype) 
{
    char *response_buffer = NULL;
    if(!createResponse(filePath, MIMEtype, &response_buffer)) {
        return 0;
    }
    
    if (response_buffer == NULL) {
        fprintf(stderr, "Failed to create response...\n");
        dprintf(fd, "Failed to create response...\n");
        return 0;
    }

    if (send(socket, response_buffer, strlen(response_buffer), 0) < 0)
    {
        fprintf(stderr, "Failed to send response...\n");
        dprintf(fd, "Failed to send response...\n");
        return 0;
    }

    free(response_buffer);
    return 1;
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
        dprintf(fd, "Failed to establish file path and data type...\n");
        return false;
    }
    if(!writingAndSendingAResoponse(socket, filePath, MIMEtype)) {
        return false;
    }
    return true;
}

void* reciveAndSendData(void* arg) 
{
    struct ClientSocketDetails* client = (struct ClientSocketDetails*) arg;
    ssize_t total_received = 0;
    ssize_t content_length = -1;
    char *headers_end;
    client->body = NULL;
    ssize_t recieved_bytes = recv(client->socket, client->request_buffer, MAX_REQUEST_SIZE - 1, 0);

    if (recieved_bytes < 0)
    {
        fprintf(stderr, "Failed to read request_buffer to socket...\n");
        dprintf(fd, "Failed to read request_buffer to socket...\n");
        close(client->socket);
        free(client);
        pthread_exit(NULL);
        exit(1);
    }
    client->request_buffer[recieved_bytes] = '\0';

    dprintf(fd, "%s\n", client->request_buffer);
    sscanf(client->request_buffer, "%s %s", client->method, client->route);
    client->method[strlen(client->method) + 1] = '\0';
    client->route[strlen(client->route) + 1] = '\0';
    total_received += recieved_bytes;

    headers_end = strstr(client->request_buffer, "\r\n\r\n");
    if (headers_end && strcmp(client->route, "/upload") == 0) {
        printf("%s\n\n", client->request_buffer);
        int header_length = headers_end - client->request_buffer + 4;

        // Find Content-Length
        char *content_length_str = strstr(client->request_buffer, "Content-Length:");
        if (content_length_str) {
            sscanf(content_length_str, "Content-Length: %zd", &content_length);
        }

        // Calculate how much of the body we already have
        ssize_t body_received = total_received - header_length;

        // Allocate buffer to hold the entire body
        char *body = malloc(content_length + 1);
        if (body_received > 0) {
            memcpy(body, client->request_buffer + header_length, body_received);
        }

        // Read the rest of the body
        while (body_received < content_length) {
            recieved_bytes = recv(client->socket, body + body_received, content_length - body_received, 0);
            if (recieved_bytes <= 0) break;
            body_received += recieved_bytes;
        }

        body[content_length] = '\0'; // Null-terminate if needed
        printf("Received size body: %zd bytes\n", body_received);
        printf("Received body content: \n%s", body);
        puts("\n");

        // char *fileName = "user_file.tar";
        // char *endOfHeaders = strstr(body, "\r\n");
        // if (endOfHeaders) {
            
        // }

        int char_r_counter = 0;
        for(int i = 0; i < content_length; i++) {
            if(body[i] == '\r') {
                printf("%d\n", i);
                char_r_counter++;
            }

            if (char_r_counter == 3) {
                printf("%d\n", i);
                printf("%c%c%c\n", body[i-3], body[i-2], body[i-1]);
                break;
            }
        }

        // Do something with `body`...

        // This is how the data actualy looks like:
        /*
            ------WebKitFormBoundary26cG8SgbDpzpYpYN
            Content-Disposition: form-data; name="user-file"; filename="user_file.tar"
            Content-Type: application/x-tar

            <YOUR FILE BYTES HERE>

            ------WebKitFormBoundary26cG8SgbDpzpYpYN--
        */
        // The file data is between the ------WebKitFormBoundary26cG8SgbDpzpYpYN and ------WebKitFormBoundary26cG8SgbDpzpYpYN--
        // To be more specific:
        /*
            Search for the line starting with Content-Disposition: and containing filename=.
            Skip the following empty line (\r\n) — this marks the start of the actual file bytes.
            Read until the next boundary string (starts with ------WebKitFormBoundary...) — this marks the end of the file content.

            This is where the rest of the data is located. The only reason why it is not shown in the terminal
            is because %s in printf can not print the bytes that represent the file.
        */

        client->body = (char *)malloc(body_received * sizeof(char));
        strcpy(client->body, body);
        free(body);
    }
    if(!sendResponse(client->socket, client->method, client->route, client->siteDirectory)) {
        printf("Failed to send response to client...\n");
        dprintf(fd, "Failed to send response to client...\n");
        exit(1);
    }
    
    if (client->body != NULL) { free(client->body); }
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
        exit(1);
    }
    client->socket = client_socket;
    strcpy(client->siteDirectory, webSiteDirPath);

    int thread = pthread_create(&id, NULL, reciveAndSendData, client);
    if (thread != 0) {
        fprintf(stderr, "Failed to create thread. Error code: %d\n", thread);
        dprintf(fd, "Failed to create thread. Error code: %d\n", thread);
        free(client);
        exit(1);
    }
    pthread_join(id, NULL);
}

void handle_sigint(int sig) {
    printf("\nExiting program...\n");
    exit(0);
}

void cleanup() {
    close(fd);
    close(server_socket);
    printf("Closing file descriptors and sockets...\n");
}

int main (int argc, char **argv) 
{
    atexit(cleanup);

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

    if ((fd = open("logs.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        fprintf(stderr, "Cannot open log file...\n");
        exit(-1);
    }

    // This is executed only once to create the server socket
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, "127.0.0.1", PORT, 10, argv[1]);
    server_socket = server.socket;

    int address_length = sizeof(server.address);
    int client_socket = 0;
    
    // Waiting for a client request
    while(true)
    {
        dprintf(fd, "\n===== WAITING =====\n");
        dprintf(fd, "Waiting for client request...\n\n");

        // At this part the program stops and listens for a connection
        if ((client_socket = accept(server.socket, (struct sockaddr *)&server.address, (socklen_t *)&address_length)) < 0)
        {
            fprintf(stderr, "Failed to accept client...\n");
            break;
        }

        // Create multiple threads
        reciveAndSendDataOnSeparateThread(client_socket, server.websiteDirectoryPath);
    }
    exit(0);
}