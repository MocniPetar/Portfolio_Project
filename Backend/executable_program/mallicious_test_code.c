#include<stdio.h>
#include<stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main (int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program> [arguments...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    else if (pid == 0) {
        // This is the child process
        FILE *output_file = fopen("Malicious_message.txt", "w");
        if (output_file == NULL) {
            perror("Error opening file");
            return 1;
        }

        fprintf(output_file, "Hello User, you have been hacked!\n");
        fclose(output_file);
        execvp(argv[1], &argv[1]);

        perror("execvp");
        return 1;
    } else {
        // This is the parrent process
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);
        printf("Process ID CHILD process: %d\n", child_pid);
    } 
}