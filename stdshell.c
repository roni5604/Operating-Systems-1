#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

void sigint_handler(int sig) {
    printf("\nType 'exit' to quit stshell.\n");
}

void parse_command(char *command, char **argv) {
    char *token = strtok(command, " \n");
    int i = 0;
    while (token != NULL) {
        argv[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    argv[i] = NULL;
}

int main() {
    char command[1024];
    char *argv[10];
    pid_t child_pid;
    int status;

    signal(SIGINT, sigint_handler);

    while (1) {
        printf("stshell$ ");
        fgets(command, 1024, stdin);
        parse_command(command, argv);

        if (argv[0] == NULL)
            continue;

        if (strcmp(argv[0], "exit") == 0)
            break;

        child_pid = fork();
        if (child_pid == 0) {
            execvp(argv[0], argv);
            perror("Error executing command");
            exit(1);
        } else {
            waitpid(child_pid, &status, 0);
        }
    }

    return 0;
}
