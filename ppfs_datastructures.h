#ifndef ppfs_ds_h
#define ppfs_ds_h

#include <fstream>
#define BLOCKSIZE 1048576

using namespace std;

class FAT{
	public:
		int noOfBlocks;
		int blockSize;
		int noOfFiles;
		
		int fnamesOffset;
		int fbOffset;
		int blocksOffset;
		
		FAT();
		FAT(int, int, int);
		void printDetails(fstream&);
};

class BLOCK{
	public:
	int bytesUsed;
	char data[BLOCKSIZE];
	int next;
	
	BLOCK();
	BLOCK(int);
  void write(fstream&);
	void read(fstream&);
};

#endif
