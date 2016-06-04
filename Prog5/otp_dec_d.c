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


int MAXBUFF = 1000000;
int startUp(const char *port);
int sendMessage(int client, char* message, int messLength);
int acceptConnection(int server);
int receiveMessage(int client, char* buffer, int maxBuff);
int decode(char* buffer);
char getChar(int num);
int getInt(char c);
void prompt();
void quit();
int keepRunning = 1;
pid_t backgroundPids[100];				//Track what is running in the background(limited to 100 for now)
int numBackground = 0;					//How many processes have been run in the background

	
main(int argc, char * argv[]){
	srand(time(NULL));
	char buffer[MAXBUFF];
	int errno, status;
	pid_t pid, sid;
	
	if(argc != 2){
		
		printf("Please use this format: otp_enc_d <PORTNUMBER>\n");
		return 1;
		
	}
	char* port = argv[1];
	int server = startUp(port);
	
	if(server == -1){
		fprintf(stderr, "Error connecting on port %s\n", port);
		return 1;
	}
	
	while(keepRunning){
		int newServer, newPort;
		printf("Waiting on port %s\n", port);
		prompt();
		
		int connection = acceptConnection(server);
		char sNewPort[100];
		
		
		

		
		
		if(connection == -1){
			fprintf(stderr, "Error: Client tried to connect but failed\n");
		}
		else if(receiveMessage(connection, buffer, sizeof(buffer)) == -1){	//Receive first message 
			fprintf(stderr, "Error: Problems receiving first message\n");
		}
		else if(strcmp(buffer, "decoder") != 0){
			//Take care of this TODO TODO
			fprintf(stderr, "An unauthorized program tried to connect\n");
			sendMessage(connection, "FAIL", 4);
			shutdown(connection, 2);
		}
		else{
			
			int i = 0;
			do{
				newPort = i + 20000 + numBackground; //20,000 - 30,000 + numBackground
				sprintf(sNewPort, "%i", newPort);
				i++;
				if(i > 10000){
					fprintf(stderr, "Error: Tried to connect on ports 20,000 - 30,000 and all failed\n");
					return 2;
				}
			}while((newServer = startUp(sNewPort)) == -1);  
			
			sendMessage(connection, "OK", sizeof("OK"));
			sendMessage(connection, sNewPort, strlen(sNewPort));
			printf("new port: %s\n", sNewPort);
			
			pid = fork();
			if(pid < 0){
				fprintf(stderr, "Error forking new process\n");
				_Exit(1);
			}	

			if(pid == 0){
				connection = acceptConnection(newServer);
				receiveFile(connection, buffer, sizeof(buffer));
				int messLength = decode(buffer);
				sendMessage(connection, buffer, messLength);
				sendMessage(connection, "*", sizeof("*"));
				_Exit(0);
			}

			if(pid > 0){
				backgroundPids[numBackground] = pid;
				numBackground++;
			}
		}


	}
}
int receiveFile(int connection, char* buffer, int length){
	
	size_t buf_idx = 0;


	while (buf_idx < MAXBUFF && 1 == read(connection, &buffer[buf_idx], 1))
	{
    if (buf_idx > 0          && 
  '*' == buffer[buf_idx])
    {
        break;
    }
    buf_idx++;
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
int decode(char* buffer){
	int i = 0;
	fprintf("%s", buffer); //for some reason this saves it?
	for(i; i < MAXBUFF; i++){
		if(buffer[i] == '\n'){
			break;
		}
	}
	int messLength = i;
	int keyStart = i + 1;
	i++; //not in enc yet
	for(i; i < MAXBUFF; i++){
		if(buffer[i] == '\n'){
			break;
		}
	}
	int keyLength = i - keyStart;
	fprintf(stderr, "Messlength: %d", messLength);
	for(i = 0; i < messLength; i++){
		int origChar = getInt(buffer[i]);
		origChar -= getInt(buffer[keyStart + i]);
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


void prompt(){											//Check if background process have exited and print the prompt
	int status = 0;
	pid_t exitpid;
	exitpid = waitpid(-1, &status, WNOHANG);
	if (exitpid > 0)
	{
		if(WIFEXITED(status)){							//Print successful exit of background pid
			int exitstatus = WEXITSTATUS(status);
			printf("Background pid %i is done: exit value %d\n", exitpid, exitstatus);
			fflush(stdout);
		}
		else if(WIFSIGNALED(status)){					//Print signal interrupt of background pid
			int signal = WTERMSIG(status);
			printf("Background pid %i was terminated by signal %d\n", exitpid, signal);
			fflush(stdout);
		}
		
		int i;
		for(i = 0; i < numBackground; i++){
			if(backgroundPids[i] == exitpid){
				backgroundPids[i] = -1;
				break;
			}
		}
		
		
	}
}

void quit(){				//Quit: kill all background process and get out of the main loop
	int i;
	for(i = 0; i < numBackground; i++){
		
		if(backgroundPids[i] != -1){
			kill(backgroundPids[i], SIGTERM);
		}
	}
	keepRunning = 0;
	return;
}
