#ifndef ppfs_write_h
#define ppfs_write_h

#include <fstream>
#include "ppfs_datastructures.h"
#include "ppfs_read.h"

using namespace std;

void writeFAT(fstream&, int);
void writeBlocks(fstream&, int);
void createFS(char*);
int removeFileName(fstream&, int, FAT);
void delFile(char*);
int blockLocation(FAT, int);
int fnameLocation(FAT, int);
int *getBlocks(fstream&, FAT, int&);
void insertFile(char*);

#endif
