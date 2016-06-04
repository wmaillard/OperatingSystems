#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/unistd.h>
#include <time.h>
#include <signal.h>
int checkFile(char *file);

main(){
	char* file = "ciphertext4";
	checkFile(file);
	
}






int checkFile(char *file){
	int length = 0;
	FILE *fp;
	fp = fopen(file, "r");
	char c;
	int newline = 0;
	while((c = fgetc(fp)) != EOF){
		length++;
		if(c == '\n'){
			newline++;
			if(newline == 2){
			printf("2 newlines\n");
			return -1;
			}
		}
		else if((c < 'A' || c > 'Z') && c != ' '){
			printf("Character failed: %c\n at %d", c, length);
			fclose(fp);
			return -1;
		}
			
	}
	fclose(fp);
	return length;
}