#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode ino;
    if (inode_iget(fs, inumber, &ino) == -1) {
        return -1;
    }

    int size = inode_getsize(&ino);
    int offsetInFile = blockNum * DISKIMG_SECTOR_SIZE;
    if (offsetInFile >= size) {
        // Si se solicita un bloque fuera del tamaño del archivo, no hay bytes válidos
        return 0;
    }
    
    int validBytes = DISKIMG_SECTOR_SIZE;
    // Caso bloque incompleto
    if (offsetInFile + DISKIMG_SECTOR_SIZE > size) {
        validBytes = size - offsetInFile;
    }
    
    // número de bloque/sector en disco correspondiente
    int block = inode_indexlookup(fs, &ino, blockNum);
    if (block == -1) {
        return -1;
    }
    
    if (diskimg_readsector(fs->dfd, block, buf) == -1) {
        return -1;
    }
    
    return validBytes;
}