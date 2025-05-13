#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
struct inode dirNode;
//inodo del directorio.
if (inode_iget(fs, dirinumber, &dirNode) < 0) {
  return -1;
}
// verifico que el inodo esté asignado y sea directorio.
if (!(dirNode.i_mode & IALLOC) || ((dirNode.i_mode & IFMT) != IFDIR)) {
  return -1;
}

int size = inode_getsize(&dirNode);
int entrySize = sizeof(struct direntv6);
int numEntries = size / entrySize;
int entriesRead = 0;
int blockNum = 0;
char buffer[DISKIMG_SECTOR_SIZE];

while (entriesRead < numEntries) {
  int bytes = file_getblock(fs, dirinumber, blockNum, buffer);
  if (bytes < 0) {
      return -1;
  }
  int entriesInBlock = bytes / entrySize;
  struct direntv6 *entries = (struct direntv6 *)buffer;
  for (int i = 0; i < entriesInBlock && entriesRead < numEntries; i++, entriesRead++) {
      // Se ignoran las entradas no asignadas
      if (entries[i].d_inumber != 0) {
          if (strncmp(entries[i].d_name, name, 14) == 0) {
              *dirEnt = entries[i];
              return 0;
          }
      }
  }
  blockNum++;
}
// No se encontró la entrada
return -1;
}
