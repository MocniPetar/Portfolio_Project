#ifndef starting_header
#define starting_header

#include "../headers/server_structures.h"

struct serverStructure createServer(int protocol, int backlog);
int listeningForRequest(struct serverStructure, char*, int);
void acceptingRequest(struct serverStructure);

#endif