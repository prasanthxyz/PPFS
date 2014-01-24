#define BLOCKSIZE 1048576

class FAT{
	public:
		int noOfBlocks;
		int blockSize;
		int noOfFiles;
		
		int fnamesOffset;
		int fbOffset;
		int blocksOffset;
		FAT(int n = 0, int bs = 0, int nof = 0){
			noOfBlocks = n;
			blockSize = bs;
			noOfFiles = nof;
			
			fnamesOffset = sizeof(FAT);
			fbOffset = fnamesOffset + noOfBlocks*(sizeof(char[30]) + sizeof(int));
			blocksOffset = fnamesOffset + fbOffset + noOfBlocks*sizeof(bool);
		}
		
		void printDetails(fstream &fs){
			printf("FAT DETAILS\n");
			printf("No. of Blocks : %d\n", noOfBlocks);
			printf("No. blockSize : %d\n", blockSize);
			printf("No. of Files  : %d\n", noOfFiles);
			fs.seekg(fbOffset);
			bool x;
			for(int i = 0; i < noOfBlocks; i++){
				printf("block %d : ", i);
				fs.read((char*)&x, sizeof(x));
				if(x)	printf("free\n");
				else 	printf("used\n");
			}
		}
};

class BLOCK{
	public:
	int bytesUsed;
	char data[BLOCKSIZE];
	int next;
		BLOCK(int s = BLOCKSIZE){
			bytesUsed = s;
			next = -1;
		}
		void write(fstream &fp);
		void read(fstream &fp);
};
void BLOCK::write(fstream &fp){
	fp.write((char*)&bytesUsed, sizeof(bytesUsed));
	for(int i = 0; i < bytesUsed; i++)
		fp.write((char*)&data[i], sizeof(char));
	fp.write((char*)&next, sizeof(next));
}
void BLOCK::read(fstream &fp){
	fp.read((char*)&bytesUsed, sizeof(bytesUsed));
	for(int i = 0; i < bytesUsed; i++){
		fp.read((char*)&data[i], sizeof(char));
	}
	fp.read((char*)&next, sizeof(next));
}
