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

int MAXBUFF = 4096;
int startUp(const char *port);
int sendMessage(int client, char* message, int messLength);
int acceptConnection(int server);
int receiveMessage(int client, char* buffer, int maxBuff);
int encode(char* buffer);
char getChar(int num);
int getInt(char c);
	
main(int argc, char * argv[]){
	
	char buffer[MAXBUFF];
	int errno;
	pid_t pid, sid;
	
	if(argc != 2){
		printf("Please use this format: otp_enc_d <PORTNUMBER>\n");
		return 1;
	}
	char* port = argv[1];
	int server = startUp(port);
	
	if(server == -1){
		printf("Error connecting on port %s\n", port);
		exit(1);
	}
	
	while(1){
		
		printf("Waiting for a connection from the client on port, %s...\n", port);
		int connection = acceptConnection(server);
		
		if(connection == -1){
			printf("Error: Client tried to connect but it failed\n");
		}
		if(receiveMessage(connection, buffer, sizeof(buffer)) == -1){	//Receive first message 
			break;
		}
		if(strcmp(buffer, "encoder\n") != 0){
			//Take care of this TODO TODO
			printf("not the right program\n");
			sendMessage(connection, "FAIL", 4);
			shutdown(connection, 2);
		}else{
			sendMessage(connection, "OK", 2);
			receiveMessage(connection, buffer, sizeof(buffer));
			int messLength = encode(buffer);
			sendMessage(connection, buffer, messLength);
			
			
		}
		

	}
}

int getInt(char c){
	if(c == ' '){
		return 26;
	}
	else{
		return c - 'A';
	}
}
char getChar(int num){
	if(num == 26){
		return ' ';
	}
	else{
		return num + 'A';
	}
}
int encode(char* buffer){
	int i = 0;
	for(i; i < sizeof(buffer); i++){
		if(buffer[i] == '\n'){
			break;
		}
	}
	int messLength = i;
	int keyStart = i + 1;
	for(i; i < sizeof(buffer); i++){
		if(buffer[i] == '\n'){
			break;
		}
	}
	int keyLength = i - keyStart;
	
	for(i = 0; i < messLength; i++){
		int origChar = getInt(buffer[i]);
		origChar += getInt(buffer[keyStart + i]);
		if(origChar < 0){
			origChar += 27;
		}
		else if(origChar > 26){
			origChar -= 27;
		}
		buffer[i] = getChar(origChar);
	}
	return messLength;
}
	
	
	



//Function: startUp
//Arguments: The port number to start a connection on
//Starts a connection on that port and returns the server number or
// -1 if startup failed (plus an error message is sent to stdout)
	
int startUp(const char *port){
	
	int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
	
	// Get address info

    //Load up hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, port, &hints, &servinfo);

    if(status != 0){
        printf("getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }

    //Get file descriptor
    int server;
    server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if(server == -1){
        printf("Error getting the file descripter: %d\n", errno);
         return -1;
    }


    //Bind socket
    if( bind(server, servinfo->ai_addr, servinfo->ai_addrlen) == -1 ){

		printf("Error binding socket: %d\n", errno);
		return -1;
    }


    //Start listening

    if( listen(server, 5) == -1 ){
         printf("Error starting to listen: %d\n", errno);
         return -1;
    }
	return server;
}
//Function: acceptConnection
//Arguments: The server number
//Accepts a connection for the server and returns the client number or
// -1 if startup failed (plus an error message is sent to stdout)
	
int acceptConnection(int server){
	
	struct sockaddr_storage clientAddr;
	int client;
	socklen_t addrSize = sizeof clientAddr;

	client = accept(server, (struct sockaddr *) &clientAddr, &addrSize);		//Accept connection

	if( client == -1 ){
		 printf("Error accepting connection: %d\n", errno);
		 return -1;
	}
	return client;
}
	
//Function: receiveMessage
//Arguments: client number, a buffer, the buffer's size
//Receives a message and puts it in buffer.
//Returns -1 if the connections has been closed and 0 on success.

int receiveMessage(int client, char* buffer, int maxBuff){
	
		if(recv(client, buffer, maxBuff, 0) != 0){
			return 0;
		}
		else{
			printf("Connection has been closed by the client\n");
			return -1;
		}
}
	
//Function: receiveMessage
//Arguments: client number
//Receives a message and puts it in buffer.
//Returns -1 on error, 0 on success, and 1 on choosing to close the connection.

int sendMessage(int client, char* message, int messLength){
			
	char buffer[MAXBUFF];
	
	buffer[0] = '\0';
	strcat(buffer, message);

	if( send(client, buffer, messLength, 0) == -1){
		printf("Error sending message to client: %d", errno);
		return -1;
	}
	
	return 0;
}