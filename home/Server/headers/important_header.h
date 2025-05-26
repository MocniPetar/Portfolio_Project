#ifndef important_header_h
#define important_header_h

#include <dirent.h>

void enqueue(struct Client*);
struct Client* dequeue();
int readContentsOfDirectory(DIR*, char*);
void printList();
int findPageDataInList(char*);
int closeFdAndFreeList();
void check(int, char*, int);

#endif