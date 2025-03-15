#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <seccomp.h>
#include <asm-generic/fcntl.h>

pid_t child_pid;

void restrict_child() 
{
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
    if (ctx == NULL) {
        perror("seccomp_init");
        exit(EXIT_FAILURE);
    }

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(access), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);

    if (seccomp_load(ctx) < 0) {
        perror("seccomp_load");
        exit(EXIT_FAILURE);
    }

    seccomp_release(ctx);
}

int main(int argc, char **argv) 
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        FILE *output_file = fopen("output.txt", "w");
        if (output_file == NULL) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        //restrict_child();

        dup2(fileno(output_file), STDOUT_FILENO);
        fclose(output_file);

        char *argv[] = {"ls", NULL};
        execvp("ls", argv);

        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        child_pid = pid;
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);

        printf("Process ID CHILD procesa: %d\n", child_pid);
        if (WIFEXITED(status)) {
            printf("CHILD proces je završio normalno s kodom izlaza: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("CHILD proces je završen signalom: %d\n", WTERMSIG(status));
        }
        return 0;
    }
}