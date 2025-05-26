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
    
    // Eliminar espacios iniciales
    while (*p && isspace(*p)) p++;
    start = p;  // Actualizar start después de saltar espacios
    
    // Si la cadena está vacía
    if (*p == '\0') {
        return;
    }
    
    // Mientras no lleguemos al final de la cadena
    while (*p) {
        // Si encontramos una comilla
        if (*p == '"') {
            if (in_quotes) {
                // Estamos cerrando comillas
                *p = '\0';  // Terminamos el argumento en la comilla
                if (p > start) {
                    args[(*arg_count)++] = start;
                }
                in_quotes = 0;
            } else {
                // Estamos abriendo comillas
                in_quotes = 1;
                // Si hay caracteres antes de las comillas, es un argumento separado
                if (p > start && *(p-1) != '\0') {
                    *(p) = '\0';
                    if (p > start) {
                        args[(*arg_count)++] = start;
                    }
                }
                start = p + 1;  // El nuevo argumento empieza después de la comilla
            }
            p++;
            // Después de cerrar comillas, saltar espacios
            if (!in_quotes) {
                while (*p && isspace(*p)) p++;
                start = p;  // El nuevo argumento empieza aquí
            }
        }
        // Si encontramos un espacio y no estamos dentro de comillas
        else if (isspace(*p) && !in_quotes) {
            *p = '\0';  // Terminar el argumento
            if (p > start) {  // Si hay contenido
                args[(*arg_count)++] = start;
            }
            p++;
            // Saltar espacios adicionales
            while (*p && isspace(*p)) p++;
            start = p;  // Iniciar nuevo argumento
        }
        // Carácter normal, avanzar
        else {
            p++;
            // Si llegamos al final
            if (*p == '\0' && start < p) {
                args[(*arg_count)++] = start;
            }
        }
    }
    
    // Si terminamos dentro de comillas, asegurarnos de incluir el último argumento
    if (in_quotes && start < p) {
        args[(*arg_count)++] = start;
    }
    
    args[*arg_count] = NULL;  // Terminar array con NULL para execvp
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");
        
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';
        
        // Verificar si el usuario quiere salir
        if (strcmp(command, "exit") == 0) {
            break;
        }

        // Reiniciar el contador de comandos
        command_count = 0;
        
        // Separar comandos por pipe
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        if (command_count == 0) {
            continue;  // Si no hay comandos, continuar
        }

        // Crear los pipes necesarios
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
            
            // Usar nuestra función mejorada para parsear argumentos
            char cmd_copy[256];
            strcpy(cmd_copy, commands[i]);  // Crear copia porque parse_args modifica la cadena
            parse_args(cmd_copy, args, &arg_count);
            
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