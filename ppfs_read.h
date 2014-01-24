#ifndef ppfs_read_h
#define ppfs_read_h

#include <sys/stat.h>
#include <iostream>
#include <cstdio>
#include "ppfs_datastructures.h"
#include "ppfs_write.h"
using namespace std;

bool accessFS(char *);
void chkFreeSpace(char *);
void listFiles(char *);
int getFileNumber(char *);
void extractFile(char *);

#endif
