#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

void sigint_handler(int sig)
{
    printf("\nType 'exit' to quit stshell.\n");
    signal(SIGINT, sigint_handler); // Reset the signal handler
}

void parse_command(char *command, char **argv)
{
    char *token = strtok(command, " \n");
    int i = 0;
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    argv[i] = NULL;
}

int contains_char(char **argv, char *ch)
{
    int index = 0;
    while (argv[index] != NULL)
    {
        if (strcmp(argv[index], ch) == 0)
            return index;
        index++;
    }
    return -1;
}

void run_cmd_with_pipe(int pipe_count, char **argv) {
    int pipe_fds[2 * pipe_count];// 2 * pipe_count because we need 2 file descriptors for each pipe
    for (int i = 0; i < pipe_count; i++) {// Create all pipes
        if (pipe(pipe_fds + i * 2) < 0) {// pipe_fds + i * 2 is the address of the first element of the pipe
            perror("Pipe failed");
            exit(1);
        }
    }

    int cmd_idx = 0;// Index of the current command
    for (int i = 0; i <= pipe_count; i++) {// Create a child process for each command
        char *cmd_argv[10];// Arguments of the current command
        int arg_idx = 0;// Index of the current argument
        while (argv[cmd_idx] != NULL && strcmp(argv[cmd_idx], "|") != 0) {// Copy arguments of the current command to cmd_argv while the current argument is not NULL and the current argument is not "|" (pipe)
            cmd_argv[arg_idx] = argv[cmd_idx];
            arg_idx++;
            cmd_idx++;
        }
        cmd_argv[arg_idx] = NULL;// Set the last argument to NULL
        cmd_idx++; // Skip "|" (pipe)

        int file_descriptor = -1;
        if (i == pipe_count) { // Check for output redirection in the last command
            int redirect_out = contains_char(cmd_argv, ">");// Check if the command contains ">"
            int append_out = contains_char(cmd_argv, ">>");// Check if the command contains ">>"
            if (redirect_out != -1) {
                cmd_argv[redirect_out] = NULL;// Set the last argument of the command to NULL
                file_descriptor = open(cmd_argv[redirect_out + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);// Open the file specified after ">" for writing
            } else if (append_out != -1) {
                cmd_argv[append_out] = NULL;
                file_descriptor = open(cmd_argv[append_out + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);// Open the file specified after ">>" for appending
            }
        }

        pid_t pid = fork();// Create a child process
        if (pid == 0) {// Child process
            // Redirect input
            if (i != 0) {
                dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO);// Redirect input from the previous pipe
            }

            // Redirect output
            if (i != pipe_count) {
                dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO);// Redirect output to the next pipe
            } else if (file_descriptor != -1) { // Redirect output to a file if specified
                dup2(file_descriptor, STDOUT_FILENO);
                close(file_descriptor);
            }

            // Close all pipe_fds
            for (int j = 0; j < 2 * pipe_count; j++) {
                close(pipe_fds[j]);
            }

            execvp(cmd_argv[0], cmd_argv);
            perror("Error executing command");
            exit(1);
        }

        if (file_descriptor != -1) {
            close(file_descriptor);
        }
    }

    // Close all pipe_fds in the parent process
    for (int i = 0; i < 2 * pipe_count; i++) {
        close(pipe_fds[i]);
    }

    // Wait for all child processes
    for (int i = 0; i <= pipe_count; i++) {
        wait(NULL);
    }
}


int main()
{
    char command[1024];
    char *argv[10];
    pid_t child_pid;// Process ID of the child process
    int status;

    signal(SIGINT, sigint_handler);// Register the signal handler

    while (1)
    {
        printf("stshell$ ");// Print the prompt
        fgets(command, 1024, stdin); // Read the command from the user
        parse_command(command, argv); // Parse the command

        if (argv[0] == NULL)    // If the command is empty,
            continue;

        if (strcmp(argv[0], "exit") == 0)// If the command is "exit",
            break;

        int redirect_out = contains_char(argv, ">");// Check if the command contains ">"
        int append_out = contains_char(argv, ">>");// Check if the command contains ">>"
        int pipe_pos = contains_char(argv, "|");// Check if the command contains "|"

        if (redirect_out != -1 || append_out != -1)// If the command contains ">" or ">>"
        {
            int file_descriptor;
            if (redirect_out != -1)
            {
                argv[redirect_out] = NULL;
                file_descriptor = open(argv[redirect_out + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            else
            {
                argv[append_out] = NULL;
                file_descriptor = open(argv[append_out + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            }

            if (file_descriptor < 0)
            {
                perror("Error opening file");
                continue;
            }

            child_pid = fork();
            if (child_pid == 0)
            {
                dup2(file_descriptor, STDOUT_FILENO);
                close(file_descriptor);
                execvp(argv[0], argv);
                perror("Error executing command");
                exit(1);
            }
            else
            {
                close(file_descriptor);
                waitpid(child_pid, &status, 0);
            }
        }
        else if (pipe_pos != -1)
        {
            int pipe_count = 0;
            for (int i = 0; argv[i] != NULL; i++)
            {
                if (strcmp(argv[i], "|") == 0)
                {
                    pipe_count++;
                }
            }

            run_cmd_with_pipe(pipe_count, argv);
        }
        else
        {
            child_pid = fork();// Create a child process
            if (child_pid == 0)
            {
                execvp(argv[0], argv);
                perror("Error executing command");
                exit(1);
            }
            else
            {
                waitpid(child_pid, &status, 0);// Wait for the child process to finish
            }
        }
    }

    return 0;
}
