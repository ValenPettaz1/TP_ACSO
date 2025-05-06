#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define POINTERS_PER_BLOCK (DISKIMG_SECTOR_SIZE / sizeof(short))
#define INODES_PER_BLOCK (DISKIMG_SECTOR_SIZE / sizeof(struct inode))

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    int inodeIndex = inumber - 1;  // Ajuste: los inodos están numerados a partir de 1
    int blockNum = INODE_START_SECTOR + (inodeIndex / INODES_PER_BLOCK);
    int offset = inodeIndex % INODES_PER_BLOCK;
    char buf[DISKIMG_SECTOR_SIZE];
    int bytesRead = diskimg_readsector(fs->dfd, blockNum, buf);
    if (bytesRead == -1) {
        return -1;
    }
    struct inode *inodeBlock = (struct inode *)buf;
    *inp = inodeBlock[offset];
    return 0;
}

/**
 * Retorna el número de bloque/sector correspondiente al enésimo bloque de datos 
 * del inodo.
 *
 * Para archivos que no son large ((i_mode & ILARG) == 0), los bloques están directamente
 * en i_addr.
 *
 * Para archivos large ((i_mode & ILARG) != 0):
 *   - i_addr[0..6] apuntan a bloques indirectos, cada uno conteniendo POINTERS_PER_BLOCK
 *     direcciones de bloques de datos.
 *   - i_addr[7] apuntan a un bloque doblemente indirecto.
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    // Caso archivo no large: direcciones directas en i_addr.
    if ((inp->i_mode & ILARG) == 0) {
        return inp->i_addr[blockNum];
    }
    
    // Archivo large
    // Caso 1: Buscamos entre los 7 bloques indirectos
    if (blockNum < 7 * POINTERS_PER_BLOCK) {
        int indirectIndex = blockNum / POINTERS_PER_BLOCK;
        int offset = blockNum % POINTERS_PER_BLOCK;
        char buf[DISKIMG_SECTOR_SIZE];
        if (diskimg_readsector(fs->dfd, inp->i_addr[indirectIndex], buf) == -1) {
            return -1;
        }
        short *indirect = (short *)buf;
        return indirect[offset];
    }
    // Caso 2: Buscamos en el bloque doblemente indirecto
    else {
        int dblBlockNum = blockNum - 7 * POINTERS_PER_BLOCK;
        int indirectRow = dblBlockNum / POINTERS_PER_BLOCK;
        int indirectOffset = dblBlockNum % POINTERS_PER_BLOCK;
        char buf[DISKIMG_SECTOR_SIZE];
        // Leer el bloque doble indirecto.
        if (diskimg_readsector(fs->dfd, inp->i_addr[7], buf) == -1) {
            return -1;
        }
        short *dblIndirect = (short *)buf;
        int indirectBlockNum = dblIndirect[indirectRow];
        // Leer el bloque indirecto referido.
        if (diskimg_readsector(fs->dfd, indirectBlockNum, buf) == -1) {
            return -1;
        }
        short *indirect = (short *)buf;
        return indirect[indirectOffset];
    }
}

int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1); 
}