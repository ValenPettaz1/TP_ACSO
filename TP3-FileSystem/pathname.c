
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (pathname == NULL || pathname[0] != '/') {
        return -1;
    }
    // Si es solo la raíz, retorna el inodo 1.
    if (strcmp(pathname, "/") == 0) {
        return 1;
    }
    // Copiamos pathname para poder tokenizar
    char *pathcopy = strdup(pathname);
    if (!pathcopy) {
        return -1;
    }
    
    int currentIno = 1;  // raíz
    char *token = strtok(pathcopy, "/");
    struct direntv6 entry;
    
    while (token != NULL) {
        // Buscar el componente token en el directorio cuyo inodo es currentIno.
        if (directory_findname(fs, token, currentIno, &entry) < 0) {
            free(pathcopy);
            return -1;
        }
        currentIno = entry.d_inumber;
        token = strtok(NULL, "/");
    }
    
    free(pathcopy);
    return currentIno;
}