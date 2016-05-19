/*William Maillard
CS 344
5/14/16
Program 4
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void prompt();
int getCommand(char arguments[512][2048]);
void execute(char arguments[512][2048], int numArgs);
void quit();
static int sigNo = 0;
pid_t backgroundPids[100];
int numBackground = 0;
int debug = 0;
void cd(char arguments[512][2048], int numArgs);
int termBySig = -1; //Was the last process terminated by a signal?
int exitOrSig = 0; //Number of signal or exit
void status();
int keepRunning = 1; //keep prompting

int main()
{

	struct sigaction termAct;
	termAct.sa_handler = SIG_IGN;
	termAct.sa_flags = 0;
	sigfillset(&(termAct.sa_mask));
	sigaction(SIGINT, &termAct, NULL);
	
	
	
	
    char arguments[512][2048];
    while(keepRunning == 1){
		if(debug == 1) printf("hey1\n");
		prompt();
		int numArguments = getCommand(arguments);
		int i = 0;

		if(strcmp(arguments[0], "exit") == 0){
			quit(arguments, numArguments);
		}
		else if(strcmp(arguments[0], "cd") == 0){
			cd(arguments, numArguments);
		}
		else if(strcmp(arguments[0], "status") == 0){
			status();
		}
		else{
			if(debug == 1) printf("hey2\n");
			execute(arguments, numArguments);
			if(debug == 1) printf("hey3\n");
		}
    }
	return;
}


void execute(char arguments[512][2048], int numArgs){
    pid_t spawnpid = -5;
    pid_t exitpid;
    int status;
    int err;
    char *firstArgs[numArgs + 1];
    int i;
    int locationSymbol;  //Location of <, >, or &
    int background = 0;  //Set if &
    int redirectBool = 0; //Set if < or >
    int allSymbols = 0; //0 if all symbols have been taken care of
    int fd, fd2;   //file descriptors for input/output redirection
	int wd = 0;		//done changing write and read for background process
	int rd = 0;
	struct sigaction termAct;
	

    if(numArgs > 0 && arguments[0][0] != '#'){

		spawnpid = fork();
		switch (spawnpid)
		{
			case -1:
			   // perror("Hull Breach!");
				exit(1);
				break;
			case 0:
				if(debug == 1) printf("I am the child!\n");
				termAct.sa_handler = SIG_DFL;
				termAct.sa_flags = 0;
				sigfillset(&(termAct.sa_mask));
				sigaction(SIGINT, &termAct, NULL);



				for(i = 0; i < numArgs; i++){
					if(strcmp(arguments[i], ">") == 0 || strcmp(arguments[i], "<") == 0 || strcmp(arguments[i], "&") == 0){
						locationSymbol = i;
						allSymbols = 1;
						break;
					}
					firstArgs[i] = &(arguments[i]);  //Add the first command to the firstArgs array
				}
				firstArgs[i] = NULL;                 //End the first args in NULL for execvp


				while(allSymbols == 1){ 
					if(strcmp(arguments[locationSymbol], "<") == 0 || (strcmp(arguments[numArgs - 1], "&") == 0 && rd == 0) ){            //Changes stdin to the correct file
						if(strcmp(arguments[numArgs - 1], "&") == 0)
							fd = open("/dev/null", O_RDONLY);
						else
							fd = open(arguments[locationSymbol + 1], O_RDONLY);

						if (fd == -1)
						{
							printf("Cannot open %s as input\n", arguments[locationSymbol + 1]);
							fflush(stdout);
							exit(1);    //Error opening file
						}
						fd2 = dup2(fd, 0);
						if (fd2 == -1)
						{
							perror("dup2"); //Error redirecting, is this correct error number?
							exit(2);
						}
						locationSymbol += 2;
						if(locationSymbol > (numArgs - 2) && strcmp(arguments[numArgs - 1], "&") != 0){
							allSymbols = 0;             //If there aren't any more symbols, then we're done
						}
						rd = 1;
					}
					else if(strcmp(arguments[locationSymbol], ">") == 0 || (strcmp(arguments[numArgs - 1], "&") == 0 && wd == 0)){            //Changes stdout to the correct file
						if(strcmp(arguments[numArgs - 1], "&") == 0)
							fd = open("/dev/null", O_WRONLY);
						else
							fd = open(arguments[locationSymbol + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);  //Flags from here: http://linux.die.net/man/3/open
						if (fd == -1)
						{
							printf("Cannot open %s for writing\n", arguments[locationSymbol + 1]);
							fflush(stdout);
							exit(1);    //Error opening file
						}
						
						if(strcmp(arguments[numArgs - 1], "&") == 0){
							dup2(fd, STDERR_FILENO);  //Redirect stderror for background processes
						}
						fd2 = dup2(fd, 1);
						if (fd2 == -1)
						{
							perror("dup2"); //Error redirecting
							exit(2);
						}

						locationSymbol += 2;
						if(locationSymbol > (numArgs - 2) && strcmp(arguments[numArgs - 1], "&") != 0){
							allSymbols = 0;             //If there aren't any more symbols, then we're done
						}

						wd = 1;
					}
					else allSymbols = 0;
				}    
				 //printf("here4\n");
				err = execvp(firstArgs[0], firstArgs);
				if(err == -1){
					int errsv = errno;
					int i;
					printf("%s: %s\n", arguments[0], strerror(errsv));
					fflush(stdout);
					exit(2);         //WHAT EXIT VALUE HERE
				}

				break;
			default:
				if(debug == 1) printf("I am the parent!\n");


				if(strcmp(arguments[numArgs - 1], "&") != 0 && numArgs > 0){
					exitpid = waitpid(spawnpid, &status, 0);
					if (exitpid == -1)
					{
						perror("wait failed");
						exit(1);
					}
					if(WIFSIGNALED(status)){
						exitOrSig = WTERMSIG(status);
						printf("Terminated by signal %d\n", exitOrSig);
						fflush(stdout);
						termBySig = 1;					//Terminated by a signal
					}
					else{
						exitOrSig = WEXITSTATUS(status);
						termBySig = 0;					//Ended normally
					}
				}
				else if(strcmp(arguments[numArgs-1], "&") == 0){
					printf("Background pid is %i\n", spawnpid);
					fflush(stdout);
					backgroundPids[numBackground] = spawnpid;
					numBackground++;
				}
				break;
			}
		fflush(stdout);
		fflush(stdin);
	}
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
    printf(":");        //print the prompt and flush
    fflush(stdout);
}



int getCommand(char arguments[512][2048]){
    size_t buffsize = 2048;
    char *input;
    int numArgs = 0;
    input = (char *)malloc(buffsize * sizeof(char));
    
    int numChars = getline(&input, &buffsize, stdin);

    if(input[numChars - 1] == '\n'){        //Remove newline at the end of getline input, some help from here: http://stackoverflow.com/questions/13000047/function-to-remove-newline-has-no-effect
        input[numChars - 1] = '\0';
    }
    
    char *pch;                      //Help from here so I don't screw up strtok: http://www.cplusplus.com/reference/cstring/strtok/
    pch = strtok (input, " ");
    
    while(pch != NULL && pch[0] != '\n'){
        strcpy(arguments[numArgs], pch);
        pch = strtok (NULL, " ");   
        numArgs++;                  //Count the number of arguments
    }
    fflush(stdin);
    return numArgs;
                                //return number of arguments
}
void quit(){
	int i;
	for(i = 0; i < numBackground; i++){
		
		if(backgroundPids[i] != -1){
			kill(backgroundPids[i], SIGKILL);
		}
	}
	keepRunning = 0;
	return;
}
void cd(char arguments[512][2048], int numArgs){
	if(numArgs > 3){
		printf("You have used too many arguments with cd\n");
		fflush(stdout);
		return;
	}
	else if(numArgs == 1){
		if(chdir(getenv("HOME")) == -1){
			perror("cd failed");
		}
	}
	else{
		if(chdir(arguments[1]) == -1){
			perror("cd failed");
		}
	
	}
	return;
}
void status(){
	if(termBySig == 0){
		printf("Exit value %d\n", exitOrSig);
		fflush(stdout);
	}
	else if(termBySig == 1){
		printf("Terminated by signal %d\n", exitOrSig);
		fflush(stdout);
	}
	else if(termBySig == -1){
		printf("Nothing has been run in the foreground yet!\n");
		fflush(stdout);
		termBySig;
	}
	return;
}
		
		

	
	
	
    
    
    