/*William Maillard
CS 372-400
Project 1
5/1/16
Description: This is the client side of a chat application that allows a client and 
server to communicate via the commandline.  More information about set-up, and command-line
arguments can be found in the README.txt file.
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

int sendMyFile(char* plainText, char* key, int connection);
int initiateContact(char *name, char* host, char* port);
int sendMessage(char *message, int server);
int receiveMessage(int server);
int checkFile(char *file);
int endSymbol(char* buff, int length);
int DEBUG = 0;

int main(int argc, char *argv[])
{


    char *name = "encoder";
    int errno;
	char *host = "localhost";
	
											//Check number of command line arguments
	if(argc != 4){
		printf("Please follow this format: otp_enc <PLAINTEXT> <KEY> <PORT>\n");
		return -1;
	}
	char *plainText = argv[1]; 	
	char *key = argv[2];
	char *port = argv[3];					
				


	int checkKey = checkFile(key);
	int checkPlain = checkFile(plainText);
	if(checkKey == -1 || checkPlain == -1 || checkPlain > checkKey){
		if(checkPlain > checkKey)
			fprintf(stderr, "Error: Your key: '%s' is too short\n", key);
		else if(checkKey == -1)
			fprintf(stderr, "Error: Invalid characters in your keyfile: '%s'\n", key);
		else
			fprintf(stderr, "Error: Invalid characters in your text file: '%s'\n", plainText);
		return 1;
	}
	
    //Most of the set up and chat interface come from Beej's guide http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
	int connection = initiateContact(name, host, port);
	if(connection == -1){
		fprintf(stderr, "Error: could not contact otp_enc_d on port %s\n", port);
		return 1;
	}

    
	int maxBuff = 256;   
    char buffer[256]; 
	
	if(recv(connection, buffer, maxBuff, 0) != 0){
		if(!(buffer[0] == 'O' && buffer[1] == 'K')){
			fprintf(stderr, "otp_enc cannot use otp_dec_d on port %s\n", port);
			return 2;
		}
		if(DEBUG == 1) printf("Buffer: %s\n", buffer);
		if(DEBUG == 1) printf("Size: %d:\n", strlen(buffer));
		strcpy(port, buffer + 3);
		port[5] = '\0';
		if(DEBUG == 1) printf("New port %s\n", port);
		if(DEBUG == 1) printf("Size: %d:\n", strlen(port));
		if(shutdown(connection, 2) != 0){
			printf("Error shutting down connection: %d", errno);
		}
		name = "";
		connection = initiateContact(name, host, port);
	}
	else{
		printf("Error communicating with server: %d", errno);
		return -1;
	}

	sendMyFile(plainText, key, connection);
	sendMessage("*", connection);
	receiveMessage(connection);
	

    if(shutdown(connection, 2) != 0){
		printf("Error shutting down connection: %d", errno);
	}
   
    return 0;
}




int checkFile(char *file){
	int length = 0;
	int fp = open(file, O_RDONLY);
	char buffer[sizeof(char)];
	int newline = 0;
	while(read(fp, &buffer, sizeof(char))){
		length++;
		if(buffer[0] == '\n'){
			newline++;
			if(newline == 2)
				return -1;
		}
		else if((buffer[0] < 'A' || buffer[0] > 'Z') && buffer[0] != ' '){
			close(fp);
			return -1;
		}
			
	}
	close(fp);
	return length;
}
	

//Function: sendMyFile
//Arguments: files and connection number
//Send a message
//Returns -1 if there was a problem and 0 on success.

int sendMyFile(char* plainText, char* key, int connection){  
	
	char* fileName;
	int i;
	for(i = 0; i < 2; i++){
		if(i == 0){
			fileName = plainText;
		}
		else fileName = key;
	
	
		int fp = open(fileName, O_RDONLY); //Open the file for reading
		int maxBuff = 1024;						
		char buffer[maxBuff];

										//The idea in this while loop was borrowed from here: http://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
		int error = 0;
		while (error == 0) {  
			int dataIn = read(fp, &buffer, sizeof(buffer));
			if (dataIn == 0){
				break;
			}
			if (dataIn < 0) {
				printf("Error reading file to send: %d\n", errno);
				return -1;
			}
			void *buffPointer = &buffer;
			while (dataIn > 0) {
				int dataOut = write(connection, buffPointer, dataIn);
				if (dataOut <= 0) {
					printf("Error writing file to socket stream: %d", errno);
					error = 1;
					break;
				}
				dataIn -= dataOut;
				buffPointer += dataOut;
			}

		}
		close(fp);
		
	}
	return 0;
	
}


//Function: initiateContact
//Arguments: The username, the host, and the port
//Gets first message from user and sends it to the server
//Returns -1 on failure or the server number on success.

int initiateContact(char *name, char* host, char* port){
	int status;
	struct addrinfo hints;
    struct addrinfo *servinfo;
	int c;
	// Get address info

    //Load up hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	char message[256];
	char buffer[256];
	int maxBuff = 256;

    status = getaddrinfo(host, port, &hints, &servinfo);

    if(status != 0){
        printf("Error with getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    //Get file descriptor
    int server;
    server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(server == -1){
         printf("Error getting the file descripter: %i\n", errno);
    }

    //Connect

    if(connect(server, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
        printf("Error connecting: %i\n", errno);
        return -1;
    }
	
	if(strcmp(name, "") != 0){
		strcpy(buffer, name);												


		if(send(server, buffer, sizeof(name), 0) == -1){
			printf("Error sending message: %i\n", errno);
			return -1;
		}
	}
	return server;
    
}

//Function: receiveMessage
//Arguments: The username and the server number
//Receives a message from the server and prints it
//with the username prepended.
//Returns 1 on success, -1 on connections closure

int receiveMessage(int server){
	int maxBuff = 500;
	char buffer[maxBuff];
	
	int numReceived = recv(server, buffer, maxBuff, 0); 
	
	while(endSymbol(buffer, maxBuff) == -1){
		printf("%.*s", numReceived, buffer);
		numReceived = recv(server, buffer, maxBuff, 0);
	}
	buffer[endSymbol(buffer, maxBuff)] = '\0';
	printf("%.*s\n", numReceived - 1, buffer);
}

int endSymbol(char* buff, int length){
	int i;
	for(i = 0; i < length; i++){
		if(buff[i] == '*')
			return i;
	}
	return -1;
}

//Function: receiveMessage
//Arguments: The username and the server number
//Get a message to send, prepend username, and send it
//Returns 1 on success, 0 on error, and -1 on connection closure

int sendMessage(char *message, int server){
	int errno;
	int maxBuff = 256;
	char buffer[maxBuff];

	
	buffer[0] = '\0';
	strcat(buffer, message);

	if(send(server, buffer, sizeof(message), 0) == -1){
		printf("Error sending message: %i", errno);
		return 0;
	}

	return 1;
}
	
	
	