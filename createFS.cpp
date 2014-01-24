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
