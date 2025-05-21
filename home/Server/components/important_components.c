#include "../headers/server_structures.h"

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(struct Client* client) 
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->client = client;
    newnode->next = NULL;
    if (tail == NULL) {
        head = newnode;
    } else {
        tail->next = newnode;
    }
    tail = newnode;
}

struct Client* dequeue()
{
    if (head == NULL) {
        return NULL;
    } else {
        struct Client* result = head->client;
        node_t *temp = head;
        head = head->next;
        if (head == NULL) {
            tail = NULL;
        }
        free(temp);
        return result;
    }
}

void check(int returnValue, char* msg, int fd)
{
    if (returnValue == -1) {
        fprintf(stderr, "%s\n", msg);
        if (fd != 0)
            dprintf(fd, "%s\n", msg);
        exit(1);
    }
}