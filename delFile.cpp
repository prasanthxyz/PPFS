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
