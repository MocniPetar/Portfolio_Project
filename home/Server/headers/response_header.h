#ifndef response_header_h
#define response_header_h

#include "../headers/server_structures.h"

int parseRequest(struct Client*);
int getReguestedRoute(char*, char*, char*);
int writingResponse(char *, char *, char **, int); 
int sendingResponse(int, char*, int);
void* constructResponse(void*);
int constructResponseOnASeparateThread(int, char*, int);

#endif