#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

pid_t child_pid;

// Signal handler za SIGINT
void sigint_handler(int signo) {
    // Pošalji SIGTERM CHILD procesu
    if (child_pid > 0) {
        kill(child_pid, SIGTERM);
    }

    // Ova linija izvršit će se samo nakon završetka CHILD procesa
    printf("Parent proces je primio SIGINT i poslao SIGTERM CHILD procesu.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    // Postavljanje signal handlera za SIGINT
    signal(SIGINT, sigint_handler);

    // Provjera broja argumenata
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <output_file> <program> [arguments...]\n", argv[0]);
        return 1;
    }

    // Stvaranje CHILD procesa
    pid_t pid = fork();

    if (pid < 0) {
        // Greška pri fork() pozivu
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Ovo je CHILD proces

        // Otvori datoteku za pisanje
        FILE *output_file = fopen(argv[1], "w");
        if (output_file == NULL) {
            perror("fopen");
            return 1;
        }

        // Preusmjeri standardni izlaz u datoteku
        dup2(fileno(output_file), STDOUT_FILENO);
        fclose(output_file);

        // Pokretanje programa u CHILD procesu
        execvp(argv[2], &argv[2]);

        // Ova linija izvršit će se samo ako execvp ne uspije
        perror("execvp");
        return 1;
    } else {
        // Ovo je PARENT proces
        child_pid = pid;

        // Čekanje na završetak CHILD procesa
        int status;
        pid_t child_pid = waitpid(pid, &status, 0);

        // Ispis informacija o CHILD procesu
        printf("Process ID CHILD procesa: %d\n", child_pid);

        // Provjera načina završetka CHILD procesa
        if (WIFEXITED(status)) {
            printf("CHILD proces je završio normalno s kodom izlaza: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("CHILD proces je završen signalom: %d\n", WTERMSIG(status));
        }

        return 0;
    }
}