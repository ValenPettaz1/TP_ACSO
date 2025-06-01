#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

// Función para parsear argumentos y manejar comillas
void parse_args(char *cmd, char *args[], int *arg_count) {
    *arg_count = 0;
    char *p = cmd;
    char *start = p;
    int in_quotes = 0;
    
    // elimina espacios iniciales
    while (*p && isspace(*p)) p++;
    start = p;  // actualizo start después de saltar espacios
    
    // si la cadena está vacía
    if (*p == '\0') {
        return;
    }
    
    while (*p) {
        if (*p == '"') {
            if (in_quotes) {
                // cerramdp las comillas
                *p = '\0';  // Terminamos el argumento en la comilla
                if (p > start) {
                    args[(*arg_count)++] = start;
                }
                in_quotes = 0;
            } else {
                // abriendo las comillas
                in_quotes = 1;
                // Si hay caracteres antes de las comillas, es un argumento separado
                if (p > start && *(p-1) != '\0') {
                    *(p) = '\0';
                    if (p > start) {
                        args[(*arg_count)++] = start;
                    }
                }
                start = p + 1;  // nuevo argumento empieza después de la comilla
            }
            p++;
            // despues de cerrar comillas, saltar espacios
            if (!in_quotes) {
                while (*p && isspace(*p)) p++;
                start = p;  
            }
        }
        // caso con un espacio y no estamos dentro de comillas
        else if (isspace(*p) && !in_quotes) {
            *p = '\0';  // terminar el argumento
            if (p > start) {  // si hay contenido
                args[(*arg_count)++] = start;
            }
            p++;
            // Salto espacios adicionales
            while (*p && isspace(*p)) p++;
            start = p; 
        }
        // caso entrada normal, avanzo
        else {
            p++;
            // Si llegamos al final
            if (*p == '\0' && start < p) {
                args[(*arg_count)++] = start;
            }
        }
    }
    
    // si termina dentro de comillas
    if (in_quotes && start < p) {
        args[(*arg_count)++] = start;
    }
    
    args[*arg_count] = NULL;  // termino array con NULL para execvp
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];

    while (1) 
    {
        printf("Shell> ");
        
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';
        
        // si el usuario quiere salir
        if (strcmp(command, "exit") == 0) {
            break;
        }

        // se reinicia el contador de comandos
        int command_count = 0;
        
        // separo comandos por pipe
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        if (command_count == 0) {
            continue;  
        }

        // se crean los pipes
        int pipes[MAX_COMMANDS - 1][2];
        for (int i = 0; i < command_count - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("Error al crear pipe");
                exit(EXIT_FAILURE);
            }
        }
        
        // array para almacenar los PIDs de los procesos hijos
        pid_t child_pids[MAX_COMMANDS] = {0};
        
        // ejecuto cada comando en un proceso hijo
        for (int i = 0; i < command_count; i++) {
            // parsing
            char *args[MAX_ARGS];
            int arg_count = 0;
            char cmd_copy[256];
            strcpy(cmd_copy, commands[i]);  // creo copia porque parse_args modifica el str
            parse_args(cmd_copy, args, &arg_count);
            
            if (arg_count == 0) continue;
            
            // lanzo proceso hijo
            pid_t pid = fork();
            
            if (pid < 0) {
                perror("Error en fork");
                exit(EXIT_FAILURE);
            }
            
            if (pid == 0) {  // Estoy en el proceso hijo
                // redirección de entrada (si no es el primer comando)
                if (i > 0) {
                    if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                        perror("Error en dup2 para stdin");
                        exit(EXIT_FAILURE);
                    }
                }
                
                // redirección de salida (si no es el último comando)
                if (i < command_count - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                        perror("Error en dup2 para stdout");
                        exit(EXIT_FAILURE);
                    }
                }
                
                // cierrp todos los descriptores de archivo de pipes
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                
                // ejecuto
                execvp(args[0], args);
                
                // si llega a esto execvp falló
                fprintf(stderr, "Error al ejecutar: %s\n", args[0]);
                exit(EXIT_FAILURE);
            } else {
                // Código del proceso padre
                child_pids[i] = pid;  // guardo el PID del hijo
            }
        }
        
        // cierro todos los pipes en el proceso padre
        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        
        // espero a que todos los procesos hijo terminen
        for (int i = 0; i < command_count; i++) {
            int status;
            pid_t child = waitpid(child_pids[i], &status, 0);
            
            if (child == -1) {
                perror("Error en waitpid");
                continue;
            }
            
            // información sobre la terminación del proceso hijo
            if (WIFEXITED(status)) {
                printf("Proceso hijo %d terminó con código %d\n", 
                       child, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Proceso hijo %d terminado por señal %d\n", 
                       child, WTERMSIG(status));
            } else {
                printf("Proceso hijo %d terminó por circunstancias desconocidas\n", 
                       child);
            }
        }
    }
    
    return 0;
}