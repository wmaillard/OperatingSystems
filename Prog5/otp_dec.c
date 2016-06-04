/*William Maillard
CS 344-400
otp_dec
6/3/16
Description: This program takes the following arguments: otp_dec <CIPHERTEXT> <KEY> <PORT> and sends
the CIPHERTEXT file and the KEY file on the PORT  to otp_dec_d which then returns a decrypted version of 
CIPHERTEXT.  This program then prints out that decrypted message.
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
int DEBUG = 0; 						//Set to 1 for a lot of messages

int main(int argc, char *argv[])
{


	char *name = "decoder*";						//This programs identifier
    int errno;
	char host[256];
	gethostname(host, sizeof(host) - 1);
	
											//Check number of command line arguments
	if(argc != 4){
		fprintf(stderr, "Please follow this format: otp_dec <CIPHERTEXT> <KEY> <PORT>\n");
		return -1;
	}
	char *plainText = argv[1]; 	
	char *key = argv[2];
	char *port = argv[3];					
				


	int checkKey = checkFile(key);					//Check that the characters in each file are correct and get the number of characters in each file if they are.
	int checkPlain = checkFile(plainText);
	if(checkKey == -1 || checkPlain == -1 || checkPlain > checkKey){	//If the characters are wrong or the key is too short, throw an error
		if(checkPlain > checkKey)
			fprintf(stderr, "Error: Your key: '%s' is too short\n", key);
		else if(checkKey == -1)
			fprintf(stderr, "Error: Invalid characters in your keyfile: '%s'\n", key);
		else
			fprintf(stderr, "Error: Invalid characters in your text file: '%s'\n", plainText);
		return 1;
	}
	
    //Most of the set up and chat interface come from Beej's guide http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
	int connection = initiateContact(name, host, port);			//Make contact with the server
	if(connection == -1){
		fprintf(stderr, "Error: could not contact otp_enc_d on port %s\n", port);
		return 2;
	}

    
	int maxBuff = 256;   
    char buffer[256]; 
	
	if(recv(connection, buffer, maxBuff, 0) != 0){  //Receive a message, if it starts with OK, then proceed
		if(!(buffer[0] == 'O' && buffer[1] == 'K')){
			fprintf(stderr, "Error: otp_dec cannot use otp_enc_d on port %s\n", port);
			return 2;
		}
		if(DEBUG == 1) printf("Buffer: %s\n", buffer);
		if(DEBUG == 1) printf("Size: %d:\n", strlen(buffer));
		
		strcpy(port, buffer + 3);				//Copy in the new port number
		port[5] = '\0';
		
		if(DEBUG == 1) printf("New port %s\n", port);
		if(DEBUG == 1) printf("Size: %d:\n", strlen(port));
		if(shutdown(connection, 2) != 0){						//Shutdown old connection
			if(DEBUG == 1)printf("Error shutting down connection: %d", errno);
		}
		name = "";
		connection = initiateContact(name, host, port);				//Connect on the new port
	}
	else{
		fprintf(stderr, "Error communicating with server: %d", errno);
		return 2;
	}

	sendMyFile(plainText, key, connection);					//Send the key and plaintext
	sendMessage("*", connection);							//End of message so send *
	receiveMessage(connection);								//Receive the encrypted message
	

    if(shutdown(connection, 2) != 0){
		if(DEBUG==1)fprintf("Error shutting down connection: %d", errno);
	}
   
    return 0;
}


//Check that the file only has one newline and only valid characters

int checkFile(char *file){
	int length = 0;
	FILE *fp = fopen(file, "r");
	char c;
	int newline = 0;
	while((c = fgetc(fp)) != EOF){
		length++;
		if(c == '\n'){
			newline++;
			if(newline == 2) return -1;
		}
		else if((c < 'A' || c > 'Z') && c != ' '){
			close(fp);
			return -1;
		}
			
	}
	fclose(fp);
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
			int dataIn = read(fp, &buffer, sizeof(buffer));			//Read in data from the file
			if (dataIn == 0){
				break;
			}
			if (dataIn < 0) {
				if(DEBUG == 1)printf("Error reading file to send: %d\n", errno);
				return -1;
			}
			void *buffPointer = &buffer;
			while (dataIn > 0) {
				int dataOut = write(connection, buffPointer, dataIn);			//Send read in data to the server
				if (dataOut <= 0) {
					if(DEBUG == 1)printf("Error writing file to socket stream: %d", errno);
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

    status = getaddrinfo(host, port, &hints, &servinfo);			//Get address info

    if(status != 0){
        if(DEBUG == 1)printf("Error with getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    //Get file descriptor
    int server;
    server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(server == -1){
         if(DEBUG == 1)printf("Error getting the file descripter: %i\n", errno);
    }

    //Connect

    if(connect(server, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
        if(DEBUG == 1)printf("Error connecting: %i\n", errno);
        return -1;
    }
	
	if(strcmp(name, "") != 0){
		strcpy(buffer, name);												


		if(send(server, buffer, sizeof(name), 0) == -1){
			if(DEBUG == 1)printf("Error sending message: %i\n", errno);
			return -1;
		}
	}
	return server;
    
}

//Function: receiveMessage
//Arguments: The server number
//Receives a message from the server and prints it
//with the username prepended.
//Returns 1 on success, -1 on connections closure

int receiveMessage(int server){
	int maxBuff = 500;
	char buffer[maxBuff];
	
	int numReceived = recv(server, buffer, maxBuff, 0); 
	
	while(endSymbol(buffer, numReceived) == -1){			//Receive a message and print it until the end symbol '*' is hit
		printf("%.*s", numReceived, buffer);
		numReceived = recv(server, buffer, maxBuff, 0);		//In this loop means that the message is longer than 500 bytes
	}
	buffer[endSymbol(buffer, numReceived)] = '\0';
	printf("%s\n", buffer);
}

//Return the location of '*', or -1 if it does not exits
int endSymbol(char* buff, int length){
	int i;
	for(i = 0; i < length; i++){
		if(buff[i] == '*')
			return i;
	}
	return -1;
}

//Function: sendMessage
//Arguments: A message pointer and the server number
//Send a message
//Returns 1 on success, 0 on error, and -1 on connection closure

int sendMessage(char *message, int server){
	int errno;
	int maxBuff = 256;
	char buffer[maxBuff];

	
	buffer[0] = '\0';
	strcat(buffer, message);

	if(send(server, buffer, sizeof(message), 0) == -1){
		if(DEBUG == 1)printf("Error sending message: %i", errno);
		return 0;
	}

	return 1;
}
	
	
	
