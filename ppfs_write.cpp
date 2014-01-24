// Contains all functions which write to the filesystem.

#include "ppfs_write.h"
#include "ppfs_datastructures.h"
#include "ppfs_read.h"

void writeFAT(fstream &fs, int noOfBlocks){
	
	//FAT DS
	//1. Number of Blocks - int
	//2. Block Size - int
	//3. Number of Files Present
	//FAT OVER
	
	//4. (fname[30] - char, startBlock - int) x number of Blocks
	//5. Free Blocks List - bool[noOfBlocks]
	
	//1,2,3
	FAT fat(noOfBlocks, BLOCKSIZE, 0);
	fs.write((char*)&fat, sizeof(fat));

	//4. (fname[30] - char, startBlock - int) x number of Blocks
	char fname[30];
	int startBlock;
	for(int i = 0; i < noOfBlocks; i++){
		fs.write((char*)&fname, sizeof(fname));
		fs.write((char*)&startBlock, sizeof(int));
	}

	//5. Free Blocks List - bool[noOfBlocks]
	bool freeBlocks[noOfBlocks];
	for(int i = 0; i < noOfBlocks; i++)
		freeBlocks[i] = true;
	fs.write((char*)&freeBlocks, sizeof(freeBlocks));
}

void writeBlocks(fstream &fs, int noOfBlocks){
	BLOCK B;
	for(int i = 0; i < noOfBlocks; i++)
		B.write(fs);
}

void createFS(char *FSname){
	int FSsize;
	fstream fs;
	printf("Enter the size of the FS (MB) : ");
	scanf(" %d", &FSsize);
	//FSsize *= 1048576;
	//FSsize = 4;
	int noOfBlocks = (FSsize*1048576)/BLOCKSIZE;
	
	fs.open(FSname, ios::out|ios::binary);
	printf("Formatting disk area as PPFS...\n");
	writeFAT(fs, noOfBlocks);
	writeBlocks(fs, noOfBlocks);
	fs.close();
	printf("File System created successfully.\n");
}

int removeFileName(fstream &fs, int n, FAT fat){
	int numbOfFilesAfterN = fat.noOfFiles-n-1;
	char fname[30];
	int ret;
	fs.seekg(fnameLocation(fat, n));
	fs.read((char*)&fname, sizeof(fname));
	printf("Deleting file '%s' ... \n", fname);
	fs.read((char*)&ret, sizeof(ret));
	
	int fnameSize = sizeof(char[30])+sizeof(int);
	int fnstrt = fnameLocation(fat, n);
	
	int strt;
	for(int i = 0; i < numbOfFilesAfterN; i++){
		fs.seekg(fnstrt+fnameSize);
		fs.read((char*)&fname, sizeof(fname));
		fs.read((char*)&strt, sizeof(strt));
		
		fs.seekp(fnstrt);
		fs.write((char*)&fname, sizeof(fname));
		fs.write((char*)&strt, sizeof(strt));
		fnstrt += fnameSize;
	}
	return ret;
}

void delFile(char *fsname){
	int fno = getFileNumber(fsname);
	
	//Read fat---------------------------------------
	fstream fs(fsname, ios::in|ios::out|ios::binary);
	FAT fat;
	fs.read((char*)&fat, sizeof(fat));
	//-----------------------------------------------
	
	//check if valid fno-----------------------------
	if(fno < 0 || fno >= fat.noOfFiles){
		printf("Invalid file number\n");
		return;
	}
	//-----------------------------------------------
	
	int startBlock = removeFileName(fs, fno, fat);
	
	//get list of blocks to delete from startBlock onwards---------
	int *blocksDeleted = new int[fat.noOfBlocks];
	int nBlocksDel = 0;
	fs.seekg(blockLocation(fat, startBlock));
	BLOCK B;
	B.read(fs);

	blocksDeleted[nBlocksDel++] = startBlock;

	while(B.next != -1){
		blocksDeleted[nBlocksDel++] = B.next;
		fs.seekg(blockLocation(fat, B.next));
		B.read(fs);
	}
	//-------------------------------------------------------------
	
	//set all blocks in the list FREE------------------------
	bool val = true;
	for(int i = 0; i < nBlocksDel; i++){
		fs.seekp(fat.fbOffset+blocksDeleted[i]*sizeof(bool));
		fs.write((char*)&val, sizeof(val));
	}
	//-------------------------------------------------------
	
	//modify and write FAT
	fat.noOfFiles--;
	fs.seekp(0);
	fs.write((char*)&fat, sizeof(fat));

	fs.close();
	printf("File deletion successfull.\n");
}

int blockLocation(FAT fat, int n){
	int blockSize = 2*sizeof(int)+sizeof(char[fat.blockSize]);
	return (fat.blocksOffset + n*blockSize);
}

int fnameLocation(FAT fat, int n = -1){
	if(n == -1)
		n = fat.noOfFiles-1;
	int fnameSize = sizeof(char[30])+sizeof(int);
	return (fat.fnamesOffset + n*fnameSize);
}

int *getBlocks(fstream &fs, FAT fat, int &n){
	fs.seekg(fat.fbOffset, ios::beg);
	int *fb = new int[fat.noOfBlocks];
	n = 0;
	bool val;
	for(int i = 0; i < fat.noOfBlocks; i++){
		fs.read((char*)&val, sizeof(val));
		if(val)
			fb[n++] = i;
	}
	return fb;
}

void insertFile(char *fsname){
	char fname[30];
	struct stat filestatus;
	
	printf("Enter the filename to insert : ");
	scanf(" %s", fname);
	ifstream fin(fname);
	if(!fin){
		printf("The file doesn't exist.\n");
		return;
	}
	
	fstream fs(fsname, ios::in|ios::out);
	FAT fat;
	fs.read((char*)&fat, sizeof(fat));

	//no. of blocks needed--------------
	stat(fname, &filestatus);
	int fsize = filestatus.st_size;
	int noFullBlocks = fsize/fat.blockSize;
	int bytesInIBlock = fsize%fat.blockSize;
	int neededBlocks = noFullBlocks;
	if(bytesInIBlock != 0)
		neededBlocks++;
	//----------------------------------
	
	
	//get no of free blocks available in FS----------
	int noOfFreeBlocks;
	int *fBlocks = getBlocks(fs, fat, noOfFreeBlocks);
	if(noOfFreeBlocks < neededBlocks){
		printf("Not enough free space available.\n");
		fs.close();
		return;
	}
	//-----------------------------------------------
	
	
	printf("Adding file '%s' ...\n", fname);
	//write file-------------------------------------
	BLOCK B;
	char data;
	int j;
	int startBlock = fBlocks[0];
	int i;
	for(i = 0; i < noFullBlocks; i++){
		for(j = 0; j < fat.blockSize; j++){
			fin.read((char*)&data, sizeof(data));
			B.data[j] = data;
		}
		B.next = fBlocks[i+1];
		fs.seekp(blockLocation(fat, fBlocks[i]));
		B.write(fs);
	}
	if(bytesInIBlock != 0){
		BLOCK NF(bytesInIBlock);
		for(j = 0; j < bytesInIBlock; j++){
			fin.read((char*)&data, sizeof(data));
			NF.data[j] = data;
		}
		fs.seekp(blockLocation(fat, fBlocks[i]), ios::beg);
		NF.write(fs);
	} else {
		i--;
		B.next = -1;
		fs.seekp(blockLocation(fat, fBlocks[i]));
		B.write(fs);
	}
	fin.close();
	//-------------------------------------------------
	
	//modify and write FAT--------------
	fat.noOfFiles++;
	fs.seekp(0, ios::beg);
	fs.write((char*)&fat, sizeof(fat));
	//----------------------------------
	
	//add the file name------------------------
	fs.seekp(fnameLocation(fat), ios::beg);
	fs.write((char*)&fname, sizeof(fname));
	fs.write((char*)&startBlock, sizeof(int));
	//-----------------------------------------
	
	//modify the free blocks list----------------
	bool val = false;
	for(i = 0; i < neededBlocks; i++){
		fs.seekp(fat.fbOffset+fBlocks[i]*sizeof(bool));
		fs.write((char*)&val, sizeof(val));
	}
	//-------------------------------------------
	
	fs.close();
	printf("File added successfully.\n");
}
