#include "../headers/server_structures.h"

node_t* head = NULL;
node_t* tail = NULL;

node_p* head_p = NULL;
node_p* tail_p = NULL;

void check(int, char* , int);
int addPageDataToList(char *, int);

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

int readContentsOfDirectory(DIR* buildDir, char * dirPath) 
{
    if (buildDir == NULL) {
        printf("(Log) Missing website build directory\n");
        return -1;
    }
    else {
        struct dirent *dir;
        int fd = 0;
        while ((dir = readdir(buildDir)) != NULL) {
            if (dir->d_type == DT_REG)
            {
                printf("\tFile: %s\n", dir->d_name);
                size_t filePathSize = strnlen(dirPath, 64) + strnlen(dir->d_name, 64);
                char *filePath = (char *)malloc(sizeof(char) * (filePathSize + 2));
                if (filePath == NULL) {
                    printf("(Log) Failed to allocate memory for deeper reading of the directory\n");
                    return -1;
                }
                memset(filePath, 0, filePathSize + 2);
                strcpy(filePath, dirPath);
                strcat(filePath, "/");
                strcat(filePath, dir->d_name);
                check((fd = open(filePath, O_RDONLY, 0644)), "(Log) Error occured while opening file\n" ,0);
                free(filePath);

                // Call addPageDataToList
                check(addPageDataToList(dir->d_name, fd), "(Log) Error occured in function addPageDataToList\n",0);
            }
            else if (dir->d_type == DT_DIR)
            {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) 
                {
                    printf("\tDirectory: %s\n", dir->d_name);

                    size_t newPathSize = strnlen(dirPath, 64) + strnlen(dir->d_name, 64);
                    char *deeperDirPath = (char *)malloc(sizeof(char) * (newPathSize + 2));
                    if (deeperDirPath == NULL) {
                        printf("(Log) Failed to allocate memory for deeper reading of the directory\n");
                        return -1;
                    }
                    memset(deeperDirPath, 0, newPathSize + 2);
                    strcpy(deeperDirPath, dirPath);
                    strcat(deeperDirPath, "/");
                    strcat(deeperDirPath, dir->d_name);

                    printf("\tDirectory path: %s\n", deeperDirPath);
                    printf("\tContents of %s:\n", dir->d_name);

                    DIR *bDir = opendir(deeperDirPath);
                    // Run a recursive call to the function
                    readContentsOfDirectory(bDir, deeperDirPath);
                    closedir(bDir);
                    free(deeperDirPath);
                }
            }
        }
    }

    return 0;
}

// Only for testing purposes
void printList() 
{
    if (head_p == NULL) {
        printf("(Log) The list is empty\n");
    } else {
        node_p* start = head_p;
        puts("\n");
        while(head_p != NULL) {
            printf("\tRoute: %s\n", head_p->route);
            printf("\tfd: %d\n", head_p->fd);
            head_p = head_p->next;
        }
        head_p = start;
    }
}

int addPageDataToList(char * route, int fd) 
{
    node_p *newnode = malloc(sizeof(node_p));
    if (newnode == NULL) {
        printf("(Log) Failed to allocate memory for the list of pages\n");
        return -1;
    }

    strcpy(newnode->route, route);
    newnode->fd = fd;
    newnode->next = NULL;

    if (tail_p == NULL) {
        head_p = newnode;
    } else {
        tail_p->next = newnode;
    }
    tail_p = newnode;
    return 0;
}

int findPageDataInList() { return 0; }

int closeFdAndFreeList() 
{ 
    if (head_p == NULL) {
        printf("(Log) Error head should not be NULL when executing method closeFdAndFreeList\n");
        return -1;
    }

    node_p* temp = NULL;
    while(head_p != NULL) 
    {
        temp = head_p;
        head_p = head_p->next;
        
        if (head_p == NULL) { tail_p = NULL; }
        close(temp->fd);
        free(temp);
    }
    printf("(Log) Successfully closed all file descriptors and freed the list\n");

    return 0;
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