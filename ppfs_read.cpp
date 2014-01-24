//  Contains all functions that read from the filesystem.

#include "ppfs_read.h"
#include "ppfs_datastructures.h"
#include "ppfs_write.h"

bool accessFS(char *FSname){
	int ch;
	printf("\n--------------------------\n");
	printf("FS operations\n");
	printf("1. List all files\n");
	printf("2. Insert a new file\n");
	printf("3. Check free space\n");
	printf("4. Extract a file\n");
	printf("5. Delete a file\n");
	printf("6. Exit\n");
	scanf(" %d", &ch);
	switch(ch){
		case 1:
			listFiles(FSname);
			break;
		case 2:
			insertFile(FSname);
			break;
		case 3:
			chkFreeSpace(FSname);
			break;
		case 4:
			extractFile(FSname);
			break;
		case 5:
			delFile(FSname);
			break;
		case 52:
			{
				FAT fat;
				fstream fs(FSname, ios::in);
				fs.read((char*)&fat, sizeof(fat));
				fat.printDetails(fs);
			}
			break;
		default:
			return false;
	}
	return true;
}

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

