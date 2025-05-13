#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define POINTERS_PER_BLOCK (DISKIMG_SECTOR_SIZE / sizeof(short))
#define INODES_PER_BLOCK (DISKIMG_SECTOR_SIZE / sizeof(struct inode))

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    int inodeIndex = inumber - 1;  // inodes numerados desde 1
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

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    // Caso archivo no large: direcciones directas 
    if ((inp->i_mode & ILARG) == 0) {
        return inp->i_addr[blockNum];
    }
    
    // Caso 1: 7 bloques simple indirectos
    if (blockNum < (int)(7 * POINTERS_PER_BLOCK)) {
        int indirectIndex = blockNum / POINTERS_PER_BLOCK;
        int offset = blockNum % POINTERS_PER_BLOCK;
        char buf[DISKIMG_SECTOR_SIZE];
        if (diskimg_readsector(fs->dfd, inp->i_addr[indirectIndex], buf) == -1) {
            return -1;
        }
        short *indirect = (short *)buf;
        return indirect[offset];
    }
    // Caso 2: bloques doblemente indirecto
    else {
        int dblBlockNum = blockNum - 7 * POINTERS_PER_BLOCK;
        int indirectRow = dblBlockNum / POINTERS_PER_BLOCK;
        int indirectOffset = dblBlockNum % POINTERS_PER_BLOCK;
        char buf[DISKIMG_SECTOR_SIZE];

        //bloque doble indirecto
        if (diskimg_readsector(fs->dfd, inp->i_addr[7], buf) == -1) {
            return -1;
        }
        short *dblIndirect = (short *)buf;
        int indirectBlockNum = dblIndirect[indirectRow];

        // bloque indirecto referido
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