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

int initiateContact(char *name, char* host, char* port);
int receiveMessage(char *name, int server);
int sendMyFile(string fileName, int connection);

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
	char *key = argv[2]
	char *port = argv[3];					//port number
    char *plainText = argv[1]; 					//host address or name


    //Most of the set up and chat interface come from Beej's guide http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
	int connection = initiateContact(name, host, port);
	if(connection == -1){
		return -1;
	}
	printf("Waiting for first reply from Server\n");
    
	int maxBuff = 256;   
    char buffer[256]; 
	
	if(recv(connection, buffer, maxBuff, 0) != 0){
		if(!(buffer[0] == 'O' && buffer[1] == 'K')){
			printf("This program is not authorized to access the requested server\n");
			return -1;
		}
	}
	else{
		printf("Error communicating with server: %d", errno);
		return -1;
	}
	
	
	sendMyFile(plainText, connection);
	

    if(shutdown(connection, 2) != 0){
		printf("Error shutting down connection: %d", errno);
	}
   
    return 0;
}


//Function: sendMyFile
//Arguments: file and connection number
//Send a message
//Returns -1 if there was a problem and 0 on success.

int sendMyFile(char* fileName, int connection){  
	
	int fp = open(fileName, O_RDONLY); //Open the file for reading
	int maxBuff = 1024;						
	char buffer[maxBuff];
									//The idea in this while loop was borrowed from here: http://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
	while (true) {  
		int dataIn = read(fp, &buffer, sizeof(buffer));
		if (dataIn == 0){
			close(fp);
			return 0;
		}
		if (dataIn < 0) {
			cout << "Error reading file to send: " << errno << endl;
			return -1;
		}
		void *buffPointer = &buffer;
		while (dataIn > 0) {
			int dataOut = write(connection, buffPointer, dataIn);
			if (dataOut <= 0) {
				cout << "Error writing file to socket stream: " << errno << endl;
			}
			dataIn -= dataOut;
			buffPointer += dataOut;
		}
	}
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

    status = getaddrinfo(gethostbyname(host), port, &hints, &servinfo);

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
	
	
	strcpy(buffer, name);												

    
    if(send(server, buffer, sizeof(name), 0) == -1){
        printf("Error sending message: %i\n", errno);
        return -1;
    }
	return server;
    
}

//Function: receiveMessage
//Arguments: The username and the server number
//Receives a message from the server and prints it
//with the username prepended.
//Returns 1 on success, -1 on connections closure

int receiveMessage(char *name, int server){
	int maxBuff = 515;
	char buffer[maxBuff];
	
	if(recv(server, buffer, maxBuff, 0) != 0){
		printf("%s\n", buffer);
		printf("%s> ", name);
		return 1;
	}
	else return -1;
}

//Function: receiveMessage
//Arguments: The username and the server number
//Get a message to send, prepend username, and send it
//Returns 1 on success, 0 on error, and -1 on connection closure

int sendMessage(char *message, int server){
	int errno;
	char message[501];
	int maxBuff = 515;
	char buffer[maxBuff];

	
	buffer[0] = '\0';
	strcat(buffer, message);

	if(send(server, buffer, sizeof(message), 0) == -1){
		printf("Error sending message: %i", errno);
		return 0;
	}

	return 1;
}
	
	
	