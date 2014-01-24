#include <sys/stat.h>
#include <iostream>
#include "listFiles.cpp"
#include "insertFile.cpp"
#include "chkFreeSpace.cpp"
#include "extractFile.cpp"
#include "delFile.cpp"

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
