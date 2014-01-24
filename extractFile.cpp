int getFileNumber(char *fsname){
	int fno;
	printf("Select the file number which you want to extract\n");
	listFiles(fsname);
	printf("Enter choice : ");
	scanf(" %d", &fno);
	fno--;
	return fno;
}
void extractFile(char *fsname){
	int fno = getFileNumber(fsname);
	
	fstream fs(fsname, ios::in|ios::binary);
	
	//extract fat---------------------
	FAT fat;
	fs.read((char*)&fat, sizeof(fat));
	//--------------------------------
	
	//check if valid fno-----------------------------
	if(fno < 0 || fno >= fat.noOfFiles){
		printf("Invalid file number\n");
		return;
	}
	//-----------------------------------------------
	
	//find the starting location of the file--
	fs.seekg(fnameLocation(fat, fno));
	char fname[30];
	int startBlock;

	fs.read((char*)&fname, sizeof(fname));
	printf("Extracting file '%s' ...\n", fname);
	fs.read((char*)&startBlock, sizeof(int));
	//----------------------------------------
	
	//extract the file
	fstream fout(fname, ios::out|ios::binary);
	fs.seekg(blockLocation(fat, startBlock));
	BLOCK B;
	B.read(fs);
	int i;
	
	while(B.next != -1){
		for(i = 0; i < B.bytesUsed; i++)
			fout.write((char*)&B.data[i], sizeof(char));
		fs.seekg(blockLocation(fat, B.next));
		B.read(fs);
	}
	for(i = 0; i < B.bytesUsed; i++)
		fout.write((char*)&B.data[i], sizeof(char));

	fout.close();
	fs.close();
	
	printf("File extracted successfully.\n");
}
