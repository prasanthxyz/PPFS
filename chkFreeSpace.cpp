void chkFreeSpace(char *fsname){
	ifstream fs(fsname);
	FAT fat;
	fs.read((char*)&fat, sizeof(fat));
	fs.seekg(fat.fbOffset);
	int count = 0;
	for(int i = 0; i < fat.noOfBlocks; i++){
		bool val;
		fs.read((char*)&val, sizeof(val));
		if(val)
			count++;
	}
	fs.close();
	printf("Amount of free space = %d MB\n", count);
}
