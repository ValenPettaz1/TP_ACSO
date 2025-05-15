#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{	
    int start, status, pid, n;
    int buffer[1];

    if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    /* Parsing of arguments */
      n = atoi(argv[1]);         // Número de procesos en el anillo
    buffer[0] = atoi(argv[2]); // Valor inicial del mensaje
    start = atoi(argv[3]);     // Proceso que inicia la comunicación
    
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
    // Validar argumentos
    if (n < 3) {
        printf("Error: Se necesitan al menos 3 procesos para formar un anillo\n");
        exit(1);
    }
    if (start < 1 || start > n) {
        printf("Error: El proceso que inicia debe estar entre 1 y %d\n", n);
        exit(1);
    }
    
    // Crear pipes para la comunicación en anillo
    int pipes[n][2];
    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Error al crear pipe");
            exit(1);
        }
    }
    
    // Pipe para comunicación entre padre y proceso inicial
    int parent_pipe[2];
    if (pipe(parent_pipe) == -1) {
        perror("Error al crear pipe para padre");
        exit(1);
    }
    
    // Crear los procesos hijos
    for (int i = 1; i <= n; i++) {
        pid = fork();
        
        if (pid < 0) {
            perror("Error al crear proceso hijo");
            exit(1);
        }
        
        if (pid == 0) { // Código del proceso hijo
            // Cerrar todos los pipes que este proceso no usará
            for (int j = 0; j < n; j++) {
                if (j != (i-1)) { // No es el pipe de entrada
                    close(pipes[j][0]);
                }
                if (j != i % n) { // No es el pipe de salida
                    close(pipes[j][1]);
                }
            }
            
            // Manejar pipe especial para el proceso inicial
            if (i != start) {
                close(parent_pipe[0]);
                close(parent_pipe[1]);
            } else {
                close(parent_pipe[1]); // El proceso inicial solo lee del padre
            }
            
            int msg;
            
            // Si es el proceso inicial, recibe del padre primero
            if (i == start) {
                read(parent_pipe[0], &msg, sizeof(int));
                close(parent_pipe[0]);
            } else {
                // Los demás procesos reciben del proceso anterior
                read(pipes[i-1][0], &msg, sizeof(int));
            }
            
            // Incrementar el mensaje
            msg++;
            printf("Proceso %d incrementó el mensaje a: %d\n", i, msg);
            
            // Enviar al siguiente proceso en el anillo
            write(pipes[i % n][1], &msg, sizeof(int));
            
            // Si es el proceso inicial, recibe el mensaje después de dar la vuelta completa
            if (i == start) {
                // Leer mensaje que ha completado el ciclo
                read(pipes[i-1][0], &msg, sizeof(int));
                printf("Proceso inicial %d recibió el mensaje final: %d\n", i, msg);
                
                // Mostrar el resultado final y salir
                printf("Resultado final: %d\n", msg);
            }
            
            // Cerrar los pipes restantes
            close(pipes[i-1][0]);
            close(pipes[i % n][1]);
            
            exit(0);
        }
    }
    
    // Código del proceso padre
    
    // Cerrar todos los pipes del anillo
    for (int i = 0; i < n; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Enviar mensaje inicial al proceso que inicia
    close(parent_pipe[0]);
    write(parent_pipe[1], buffer, sizeof(int));
    close(parent_pipe[1]);
    
    // Esperar a que todos los hijos terminen
    for (int i = 0; i < n; i++) {
        wait(&status);
    }
    
    return 0;
}