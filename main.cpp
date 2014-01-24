#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "ppfs_datastructures.h"
#include "ppfs_read.h"
#include "ppfs_write.h"

using namespace std;

int main(){
	char FSname[30];
	//strcpy(FSname, "pp.hdd");
	printf("Enter the name of the file_system : ");
	scanf(" %s", FSname);

	ifstream check(FSname);
	if(!check){
		createFS(FSname);
	}
	check.close();
	while(accessFS(FSname));
	return 0;
}
