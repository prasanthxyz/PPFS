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
