#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode ino;
    // Obtener el inodo
    if (inode_iget(fs, inumber, &ino) == -1) {
        return -1;
    }

    int size = inode_getsize(&ino);
    // Calculamos el offset del bloque en el archivo
    int offsetInFile = blockNum * DISKIMG_SECTOR_SIZE;
    if (offsetInFile >= size) {
        // Si se solicita un bloque fuera del tamaño del archivo, no hay bytes válidos
        return 0;
    }
    
    int validBytes = DISKIMG_SECTOR_SIZE;
    // Ajustar el número de bytes válidos en caso de ser el último bloque incompleto
    if (offsetInFile + DISKIMG_SECTOR_SIZE > size) {
        validBytes = size - offsetInFile;
    }
    
    // Obtener el número de bloque/sector en disco correspondiente
    int block = inode_indexlookup(fs, &ino, blockNum);
    if (block == -1) {
        return -1;
    }
    
    if (diskimg_readsector(fs->dfd, block, buf) == -1) {
        return -1;
    }
    
    return validBytes;
}