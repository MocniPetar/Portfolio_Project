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

int readContentsOfDirectory(DIR* buildDir, char * dirPath) 
{
    if (buildDir == NULL) {
        printf("(Log) Missing website build directory\n");
        return -1;
    }
    else {
        struct dirent *dir;
        while ((dir = readdir(buildDir)) != NULL) {
            if (dir->d_type == DT_REG)
            {
                printf("\tFile: %s\n", dir->d_name);

                // Call addPageDataToList
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

int addPageDataToList() {
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