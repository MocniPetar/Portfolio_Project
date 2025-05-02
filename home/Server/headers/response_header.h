#ifndef response_header
#define response_header

#include "../headers/server_structures.h"

int parseRequest(struct Client*);
int getReguestedRoute(char*, char*, char*);
int writingResponse(char *, char *, char **, int); 
int sendingResponse(int, char*, int);
void* constructResponse(void*);
int constructResponseOnASeparateThread(int, char*, int);

#endif