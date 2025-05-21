#include "../headers/server_structures.h"
#include "../headers/response_header.h"
#include "../headers/important_header.h"

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

struct serverStructure createServer(int, int);
void enqueue(struct Client*);
struct Client* dequeue();
int acceptingRequest(struct serverStructure);
void* thread_function(void *);
void listeningForRequest(struct serverStructure, char*, int); 

struct serverStructure createServer(int protocol, int backlog) 
{
    struct serverStructure server;

    bzero(&server.address, sizeof(server.address));
    server.address.sin_family = AF_INET;
    server.address.sin_port = htons(PORT);
    server.address.sin_addr.s_addr = htonl(INADDR_ANY);

    check((server.socket = socket(PF_INET, SOCK_STREAM, protocol)), "(Log) Failed to connect socket", 0);
    check(bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)), "(Log) Failed to bind socket", 0);
    check(listen(server.socket, backlog), "(Log) Failed to start listening", 0);
    
    char hostBuffer[NI_MAXHOST], serviceBuffer[NI_MAXSERV];
    int error = getnameinfo((struct sockaddr *)&server.address, sizeof(server.address), hostBuffer, sizeof(hostBuffer), serviceBuffer, sizeof(serviceBuffer), 0);
    
    if (error != 0) {
        printf("(Log) Error: %s\n", gai_strerror(error));
        exit(1);
    }

    printf("\n(Log) Server is listening on http://%s:%d/\n\n", hostBuffer ,ntohs(server.address.sin_port));
    return server;
}

int acceptingRequest(struct serverStructure server)
{
    int socket = 0;
    int address_length = sizeof(server.address);
    char client_address[256];

    if ((socket = accept(server.socket, (struct sockaddr *)&server.address, (socklen_t *)&address_length)) == -1) {
        return -1;
    }

    inet_ntop(AF_INET, &server.address, client_address, 256);
    // printf("(Log) Client connected from IP address: %s\n", client_address);

    return socket;
}

void* thread_function(void *arg) 
{
    while(true)
    {
        struct Client* client;
        pthread_mutex_lock(&lock);
        if ((client = dequeue()) == NULL) {
            pthread_cond_wait(&condition_var, &lock);
            // try again
            client = dequeue();
        }
        pthread_mutex_unlock(&lock);

        if (client != NULL) {
            constructResponse(client);
        }
    }
}

void listeningForRequest(struct serverStructure server, char* dir, int fd) 
{
    int clientSocket = 0;
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], NULL, &thread_function, NULL);
    }

    // Waiting for client request
    while(true)
    {
        dprintf(fd, "\n===== WAITING =====\n");
        check((clientSocket = acceptingRequest(server)), "(Log) Failed to accept client request", fd);
        
        struct Client* pclient = (struct Client*)malloc(sizeof(struct Client));

        pclient->fd = fd;
        strcpy(pclient->dir, dir);
        pclient->socket = clientSocket;

        pthread_mutex_lock(&lock);
        enqueue(pclient);
        pthread_cond_signal(&condition_var);
        pthread_mutex_unlock(&lock);
    }
}