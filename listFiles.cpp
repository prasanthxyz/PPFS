void listFiles(char *fsname){
	fstream fs(fsname, ios::in|ios::binary);
	FAT fat;
	char fname[30];
	int start;
	fs.read((char*)&fat, sizeof(fat));
	
	if(fat.noOfFiles == 0){
		printf("No files present in the FS\n");
		return;
	}
	
	fs.seekg(fat.fnamesOffset, ios::beg);
	
	for(int i = 0; i < fat.noOfFiles; i++){
		fs.read((char*)&fname, sizeof(fname));
		fs.read((char*)&start, sizeof(start));
		printf("%d. %s\n",i+1, fname);
	}
	fs.close();
}
