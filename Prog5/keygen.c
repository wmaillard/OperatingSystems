/*William Maillard
CS 344-400
keygen
6/3/16
Description: This takes the arguments keygen <KEYLENGTH> and generates a random key of the given KEYLENGTH.  It then prints out the key.
*/


#include <time.h>
#include <stdlib.h>
#include <stdio.h>


char getChar(int num);

int main(int argc, char *argv[]){
	srand(time(NULL));							//Set the seed
	if(argc != 2){								//Check the number of arguments
		printf("Please follow this format: keygen <KEYLENGTH>\n");
		return -1;
	}
	else{
		int end = atoi(argv[1]);					//Convert the number of arguments to an int
		int i = 0;
		for(i; i < end; i++){
			int random = rand() % 27;				//Get a random int
			char c = getChar(random);				//Convert the int
			printf("%c", c);
		}
		printf("\n");
	}
	return 0;
}


char getChar(int num){
	if(num == 26){								//Convert an into into our chars
		return ' ';
	}
	else{
		return num + 'A';
	}
}