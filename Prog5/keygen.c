#include <time.h>
#include <stdlib.h>
#include <stdio.h>


char getChar(int num);

int main(int argc, char *argv[]){
	srand(time(NULL));
	if(argc != 2){
		printf("Please follow this format: keygen <KEYLENGTH>\n");
		return -1;
	}
	else{
		int end = atoi(argv[1]);
		int i = 0;
		for(i; i < end; i++){
			int random = rand() % 27;
			char c = getChar(random);
			printf("%c", c);
		}
		printf("\n");
	}
	return 0;
}


char getChar(int num){
	if(num == 26){
		return ' ';
	}
	else{
		return num + 'A';
	}
}