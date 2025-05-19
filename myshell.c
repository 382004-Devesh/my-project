/**
 * SimpleShell - A beginner-friendly shell with background processes
 * 
 * This program creates a command-line shell that can:
 * 1. Show a prompt
 * 2. Take user input
 * 3. Run commands in the background using ampersand (&)
 * 4. Support built-in commands (exit, cd, help)
 * 
 * Compile with: gcc -o myshell simple_shell.c
 * Run with: ./myshell
 */

#include <stdio.h>    /* For input/output functions */
#include <stdlib.h>   /* For memory allocation */
#include <string.h>   /* For string functions */
#include <unistd.h>   /* For fork, exec, etc. */
#include <sys/wait.h> /* For wait functions */
#include <errno.h>    /* For error handling */

/* Maximum size for user input */
#define MAX_INPUT_SIZE 1024

/* Maximum number of command arguments */
#define MAX_ARGS 64

/* Shell prompt to display */
#define PROMPT "myshell> "

/* Function prototype */
void execute_command(char *cmd);

/* Main function - this is where our program starts */
int main() {
    /* Welcome message */
    printf("Welcome to SimpleShell! Type 'help' for commands, 'exit' to quit.\n");
    
    /* Variable for user input */
    char input[MAX_INPUT_SIZE];
    
    /* Main loop - keeps running until the user exits */
    while (1) {
        /* Step 1: Display the prompt */
        printf("%s", PROMPT);
        
        /* Step 2: Read user input */
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            printf("\n");
            break;  /* Exit if we can't read input (e.g., Ctrl+D) */
        }
        
        /* Step 3: Execute the command */
        execute_command(input);
    }
    
    return 0;
}

/**
 * Execute a single command
 */
void execute_command(char *cmd) {
    char *args[MAX_ARGS];        /* To store command arguments */
    int background = 0;          /* Flag for background execution */
    
    /* Remove trailing newline */
    cmd[strcspn(cmd, "\n")] = 0;
    
    /* Trim leading and trailing spaces */
    while (*cmd == ' ') cmd++;
    int len = strlen(cmd);
    while (len > 0 && cmd[len-1] == ' ') {
        cmd[len-1] = 0;
        len--;
    }
    
    /* If empty command, do nothing */
    if (strlen(cmd) == 0) {
        return;
    }
    
    /* Check for background execution (& at the end) */
    if (strchr(cmd, '&') != NULL) {
        background = 1;
        /* Remove the & character */
        char *ampPos = strchr(cmd, '&');
        *ampPos = '\0';
        
        /* Trim any spaces before the & */
        len = strlen(cmd);
        while (len > 0 && cmd[len-1] == ' ') {
            cmd[len-1] = 0;
            len--;
        }
    }
    
    /* Split the command into arguments */
    int i = 0;
    args[i] = strtok(cmd, " ");
    while (args[i] != NULL && i < MAX_ARGS - 1) {
        args[++i] = strtok(NULL, " ");
    }
    args[i] = NULL;  /* The last argument must be NULL for execvp */
    
    /* If no valid command after parsing, do nothing */
    if (args[0] == NULL) {
        return;
    }
    
    /* Check for built-in commands */
    
    /* 'exit' command - quit the shell */
    if (strcmp(args[0], "exit") == 0) {
        printf("Goodbye!\n");
        exit(0);
    }
    
    /* 'cd' command - change directory */
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            /* If no directory is specified, go to the home directory */
            chdir(getenv("HOME"));
        } else {
            /* Change to the specified directory */
            if (chdir(args[1]) != 0) {
                printf("Error: Could not change to directory '%s'\n", args[1]);
            }
        }
        return;
    }
    
    /* 'help' command - show help information */
    if (strcmp(args[0], "help") == 0) {
        printf("SimpleShell - Available commands:\n");
        printf("  cd [directory]     - Change to the specified directory\n");
        printf("  help               - Show this help message\n");
        printf("  exit               - Exit the shell\n");
        printf("  command &          - Run a command in the background\n");
        printf("  Any other command will be executed as a program\n");
        return;
    }
    
    /* For external commands, create a new process */
    int pid = fork();
    
    if (pid < 0) {
        /* Fork failed */
        printf("Error: Could not create a new process\n");
    } else if (pid == 0) {
        /* Child process - execute the command */
        execvp(args[0], args);
        
        /* If execvp returns, it means there was an error */
        printf("Error: Command '%s' not found\n", args[0]);
        exit(1);
    } else {
        /* Parent process */
        if (!background) {
            /* Wait for the child to finish if not running in background */
            waitpid(pid, NULL, 0);
        } else {
            /* Print process ID for background processes */
            printf("[Background] Process ID: %d\n", pid);
        }
    }
}
