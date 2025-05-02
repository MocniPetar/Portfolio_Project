#include "headers/Server.h"

#define PORT 8080

int server_socket = 0;
int fd = 0;

struct Server server_constructor(int protocol, int backlog)
{
    struct Server server;

    server.service = SOCK_STREAM;
    server.protocol = protocol;
    server.backlog = backlog;

    server.address.sin_family = AF_INET;
    server.address.sin_port = htons(PORT);
    server.address.sin_addr.s_addr = htonl(INADDR_ANY);

    server.socket = socket(PF_INET, server.service, server.protocol);

    if (server.socket < 0)
    {
        perror("(Log) Failed to connect socket...\n");
        exit(1);
    }

    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
    {
        perror("(Log) Failed to bind socket...\n");
        exit(1);
    }

    if (listen(server.socket, server.backlog) < 0)
    {
        perror("(Log) Failed to start listening...\n");
        exit(1);
    }
    
    char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
    int error = getnameinfo((struct sockaddr *)&server.address, sizeof(server.address), hostBuffer, sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);
    
    if (error != 0) {
        printf("(Log) Error: %s\n", gai_strerror(error));
        exit(1);
    }

    printf("\n(Log) Server is listening on http://%s:%d/\n\n", hostBuffer ,ntohs(server.address.sin_port));
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

bool establishingFilePathAndDataType(char *filePath, char *method, char *MIMEtype) 
{   
    // if (strcmp(method, "POST") == 0 && strcmp(route, "/upload") == 0) {
    //     // Call a function to parse the data from body
    //     return false;
    // }

    if (strcmp(method, "GET") != 0)
    {
        printf("%s is an unacceptable method...\n", method);
        return false;
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
            return 0;
        }

        if(fread(file_buffer, 1, file_size, file) <= 0) {
            fprintf(stderr, "(Log) Failed to read data from file...\n");
            dprintf(fd, "(Log) Failed to read data from file...\n");
            free(file_buffer);
            fclose(file);
            return 0;
        }
        file_buffer[file_size] = '\0';

        *response_buffer = (char *)malloc(response_setup_size + file_size + 1);
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
        fprintf(stderr, "(Log) Failed to create response...\n");
        dprintf(fd, "(Log) Failed to create response...\n");
        return 0;
    }

    if (send(socket, response_buffer, strlen(response_buffer), 0) < 0)
    {
        fprintf(stderr, "(Log) Failed to send response...\n");
        dprintf(fd, "(Log) Failed to send response...\n");
        return 0;
    }

    free(response_buffer);
    return 1;
}

bool sendResponse(int socket, char* method, char* route)
{
    char filePath[1024];
    char MIMEtype[16];
    snprintf(filePath, sizeof(filePath), "%s", route);

    // Just send filePath, method and MIMEType
    if(!establishingFilePathAndDataType(filePath, method, MIMEtype)) 
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

int parseRequestBody(struct ClientSocketDetails* client, ssize_t total_received, char* headers_end) {
    ssize_t content_length = -1;
    ssize_t recieved_bytes;

    printf("(ssize_t)strlen(client->request_buffer): %lu\n", (ssize_t)strlen(client->request_buffer));
    printf("(ssize_t)strlen(headers_end): %lu\n", (ssize_t)strlen(headers_end));

    int header_length = (ssize_t)strlen(client->request_buffer) - (ssize_t)strlen(headers_end) + 4;

    char *content_length_str = strstr(client->request_buffer, "Content-Length:");
    if (content_length_str) {
        sscanf(content_length_str, "Content-Length: %zd", &content_length);
    }
    printf("header_length: %d\n", header_length);
    printf("content_length: %lu\n", content_length);
    printf("total_received: %lu\n", total_received);

    ssize_t body_received = total_received - header_length;
    printf("body_received: %lu\n", body_received);

    char *body = malloc(content_length + 1);
    if (body_received > 0) {
        memcpy(body, client->request_buffer + header_length, body_received);
    }

    while (body_received < content_length) {
        recieved_bytes = recv(client->socket, body + body_received, content_length - body_received, 0);
        if (recieved_bytes <= 0) break;
        body_received += recieved_bytes;
    }
    body[content_length] = '\0';    

    FILE* body_file = fopen("body.tar", "wb");
    if (body_file == NULL) {
        printf("Failed to create file...\n");
        close(client->socket);
        free(client);
        return -1;
    }
    printf("Received size body: %zd bytes\n", body_received);
    printf("Body:\n%s\n", body);

    char *fileName = strstr(body, "filename=");
    char* crlf_location_before_data = NULL;
    char* crlf_location_after_data = NULL;
    if (fileName) {
        printf("1\n");
        crlf_location_before_data = strstr(fileName, "\r\n\r\n");

        if(crlf_location_before_data) {
            printf("1.1\n");
            crlf_location_after_data = strstr(crlf_location_before_data + 4, "\r\n\r\n");
        }

        if (crlf_location_after_data) {
            printf("1.2\n");
            size_t size_of_data = strlen(crlf_location_before_data) - strlen(crlf_location_after_data - 4);
            if(fwrite(crlf_location_before_data + 4, 1, size_of_data, body_file) < size_of_data) {
                printf("Failed to write the contents to the file...\n");
                fclose(body_file);
                free(body);
                return -1;
            }
        }
    }
    else {
        printf("2\n");
        memcpy(body, "No body", strlen("No body"));
        body[strlen("No body") + 1] = '\0';
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
    fclose(body_file);
    return 0;
}

void* reciveAndSendData(void* arg)
{
    struct ClientSocketDetails* client = (struct ClientSocketDetails*) arg;

    /* At this stage this code is not needed! */
    // ssize_t total_received = 0;
    // char *headers_end;
    // client->body = NULL;

    ssize_t recieved_bytes = recv(client->socket, client->request_buffer, MAX_REQUEST_SIZE - 1, 0);
    client->request_buffer[recieved_bytes] = '\0';

    if (recieved_bytes < 0)
    {
        fprintf(stderr, "(Log) Failed to read request_buffer to socket...\n");
        dprintf(fd, "(Log) Failed to read request_buffer to socket...\n");
        free(client);
        return NULL;
    }

    dprintf(fd, "%s\n", client->request_buffer);
    sscanf(client->request_buffer, "%s %s", client->method, client->route);
    client->method[strlen(client->method) + 1] = '\0';
    client->route[strlen(client->route) + 1] = '\0';

    ssize_t dirLength = strlen(client->siteDirectory);
    ssize_t routeLength = strlen(client->route);

    char* temp_route = (char *)malloc(routeLength + dirLength + 1);
    if (temp_route == NULL) {
        printf("(Log) Failed to allocate memory using malloc for temp_route...\n");
        free(client);
        return NULL;
    }

    strcpy(temp_route, client->siteDirectory);
    strcat(temp_route, client->route);
    temp_route[routeLength + dirLength] = '\0';
    strcpy(client->route, temp_route);

    /* At this stage this code is not needed! */
    // total_received += recieved_bytes;
    // headers_end = strstr(client->request_buffer, "\r\n\r\n");
    // if (headers_end && strcmp(client->route, "/upload") == 0) {
    //     if(parseRequestBody(client, total_received, headers_end) == -1) {
    //         printf("Failed to parse request body...\n");
    //         dprintf(fd, "Failed to parse request body...\n");
    //     }
    // }
    
    if(!sendResponse(client->socket, client->method, client->route)) {
        printf("(Log) Failed to send response to client...\n");
        dprintf(fd, "(Log) Failed to send response to client...\n");
    }
    
     /* At this stage this code is not needed! */
    // if (client->body != NULL) { free(client->body); }

    free(client);
    free(temp_route);
    return NULL;
}

int reciveAndSendDataOnSeparateThread(int client_socket, char *webSiteDirPath) 
{
    struct ClientSocketDetails* client = (struct ClientSocketDetails*)malloc(sizeof(struct ClientSocketDetails));
    
    if (client == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return -1;
    }
    
    client->socket = client_socket;
    strcpy(client->siteDirectory, webSiteDirPath);
    client->siteDirectory[strlen(client->siteDirectory) + 1] = '\0';
    
    pthread_t thread;
    int result = pthread_create(&thread, NULL, reciveAndSendData, client);
    if (result != 0) {
        fprintf(stderr, "Failed to create thread. Error code: %ld\n", thread);
        dprintf(fd, "Failed to create thread. Error code: %ld\n", thread);
        free(client);
        return -1;
    }

    pthread_join(thread, NULL);
    return 0;
}

void handle_sigint(int sig) {
    printf("\nExiting program...\n");
    exit(0);
}

void cleanup() {
    dprintf(fd, "End of session...\n");
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
    if (dir) {
        closedir(dir);
    } else {
        printf("Failed to locate/open project build directory...\n");
        exit(1);
    }

    printf("\nSuccessfully located the project build directory...\n");

    if ((fd = open("./logs/output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        fprintf(stderr, "Cannot open log file...\n");
        exit(-1);
    }

    // This is executed only once to create the server socket
    struct Server server = server_constructor(0, 10);
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
        printf("(Log) Client Socket: %d\n", client_socket);

        // Create multiple threads
        if(reciveAndSendDataOnSeparateThread(client_socket, argv[1]) == -1) {
            close(client_socket);
            exit(-1);
        }
        close(client_socket);
    }
    exit(0);
}