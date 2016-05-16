/*William Maillard
CS 344
5/14/16
Program 4
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

void prompt();
int getCommand(char arguments[512][2048]);
void execute(char arguments[512][2048], int numArgs);

int main()
{
    char arguments[512][2048];
    while(1){
    prompt();
    int numArguments = getCommand(arguments);
    int i = 0;
    
    if(strcmp(arguments[0], "exit") == 0){
    }
    else if(strcmp(arguments[0], "cd") == 0){
    }
    else if(strcmp(arguments[0], "status") == 0){
    }
    else{
        execute(arguments, numArguments);
    }
    
 /*   for(i; i < numArguments; i++){
        printf("%s: %i\n", arguments[i], i);
        
    }*/
    }
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
    
    spawnpid = fork();
    switch (spawnpid)
    {
        case -1:
           // perror("Hull Breach!");
            exit(1);
            break;
        case 0:
            //printf("I am the child!\n");
            for(i = 0; i < numArgs; i++){
                if(strcmp(arguments[i], ">") == 0 || strcmp(arguments[i], "<") == 0 || strcmp(arguments[i], "&") == 0){
                    locationSymbol = i;
                    i++;
                    allSymbols = 1;
                    break;
                }
                firstArgs[i] = &(arguments[i]);  //Add the first command to the firstArgs array
            }
            firstArgs[i] = NULL;                 //End the first args in NULL for execvp
           
            
            while(allSymbols == 1){ 

                if(strcmp(arguments[locationSymbol], "<") == 0){            //Changes stdin to the correct file
                    fd = open(arguments[locationSymbol + 1], O_RDONLY);
                    if (fd == -1)
                    {
                        perror("open");
                        exit(1);    //Error opening file, how should I handle this?
                    }
                    fd2 = dup2(fd, 0);
                    if (fd2 == -1)
                    {
                        perror("dup2"); //Error redirecting, how should I handle this?
                        exit(2);
                    }
                    locationSymbol += 2;
                    if(locationSymbol > (numArgs - 2) && strcmp(arguments[locationSymbol], "&") != 0){
                        allSymbols = 0;             //If there aren't any more symbols, then we're done
                    }
				}
                else if(strcmp(arguments[locationSymbol], ">") == 0){            //Changes stdout to the correct file
                    
					fd = open(arguments[locationSymbol + 1], O_WRONLY, O_TRUNC, O_CREAT);
                    if (fd == -1)
                    {
                        perror("open");
                        exit(1);    //Error opening file, how should I handle this?
                    }
                    fd2 = dup2(fd, 1);
                    if (fd2 == -1)
                    {
                        perror("dup2"); //Error redirecting, how should I handle this?
                        exit(2);
                    }
                    
                    locationSymbol += 2;
                    if(locationSymbol > (numArgs - 2) && strcmp(arguments[locationSymbol], "&") != 0){
                        allSymbols = 0;             //If there aren't any more symbols, then we're done
                    }
                }
                else if(strcmp(arguments[locationSymbol], "&") == 0){
                  	background = 1;
                }
				else allSymbols = 0;
            }
                    
                    
             
            err = execvp(firstArgs[0], firstArgs);
            if(err == -1){
                int errsv = errno;
                printf("Error executing %s: %s\n", arguments[0], strerror(errsv));
            }
         
            break;
        default:
           // printf("I am the parent!");
            exitpid = waitpid(spawnpid, &status, 0);
            if (exitpid == -1)
            {
                perror("wait failed");
                exit(1);
            }
            if (WIFEXITED(status))
            {
               // printf("The process exited normally\n");
                int exitstatus = WEXITSTATUS(status);
               // printf("exit status was %d\n", exitstatus);
            }
            else
                // printf("Child terminated by a signal\n");
            break;
    }                
}


void prompt(){
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
    
    
    