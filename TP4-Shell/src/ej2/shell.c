#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';
        
        // Verificar si el usuario quiere salir
        if (strcmp(command, "exit") == 0) {
            break;
        }

        // Reiniciar el contador de comandos
        command_count = 0;
        
        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        if (command_count == 0) {
            continue;  // Si no hay comandos, continuar
        }

        // Crear los pipes necesarios (uno menos que el número de comandos)
        int pipes[MAX_COMMANDS - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("Error al crear pipe");
                exit(EXIT_FAILURE);
            }
        }
        
        // Ejecutar cada comando en un proceso hijo
        pid_t pids[MAX_COMMANDS];
        for (int i = 0; i < command_count; i++) {
            // Parsear el comando en programa y argumentos
            char *args[MAX_ARGS];
            int arg_count = 0;
            
            // Eliminar espacios iniciales
            char *cmd = commands[i];
            while (*cmd == ' ') cmd++;
            
            // Tokenizar por espacios para separar argumentos
            char *arg = strtok(cmd, " ");
            while (arg != NULL && arg_count < MAX_ARGS - 1) {
                args[arg_count++] = arg;
                arg = strtok(NULL, " ");
            }
            args[arg_count] = NULL;  // Terminar array con NULL para execvp
            
            if (arg_count == 0) continue;
            
            // Crear proceso hijo
            pids[i] = fork();
            
            if (pids[i] < 0) {
                perror("Error en fork");
                exit(EXIT_FAILURE);
            }
            
            if (pids[i] == 0) {  // Código del proceso hijo
                // Configurar redirección de entrada (si no es el primer comando)
                if (i > 0) {
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }
                
                // Configurar redirección de salida (si no es el último comando)
                if (i < command_count - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                
                // Cerrar todos los descriptores de archivo de pipes
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                
                // Ejecutar el comando
                execvp(args[0], args);
                
                // Si llegamos aquí, execvp falló
                fprintf(stderr, "Error al ejecutar: %s\n", args[0]);
                exit(EXIT_FAILURE);
            }
        }
        
        // Cerrar todos los pipes en el proceso padre
        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        
        // Esperar a que todos los procesos hijo terminen
        for (int i = 0; i < command_count; i++) {
            waitpid(pids[i], NULL, 0);
        }
        
        // Reiniciar para el próximo comando
        command_count = 0;
    }
    
    return 0;
}