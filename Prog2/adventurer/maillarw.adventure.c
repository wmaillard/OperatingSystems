/*William Maillard
CS 344
5/1/16
Program 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char *createFiles();
void makeRoom (char dirName[80], int theRooms[7], int section, int name);
char *userInterface(char *curRoom, char *connections);
char *moveLocation(char *curRoom);
char *getLastWord(char *buff);
char *getDirectory();

int main()
{
    int count = 0;  									//Number of rooms visited

    char *curRoom = createFiles();						//Create the room files and store the first room
    
    char order[256];  									//Create a file to store the order of rooms visited in
    sprintf(order, "%s/tempPathFile", getDirectory());

    FILE *fp;
    fp = fopen(order, "w+");

    do{													//Change curRoom to the room returned by moveLocation starts with '!' 
        curRoom = moveLocation(curRoom);				//which means that the last room has been reached
		if(curRoom[0] != '!'){
			fprintf(fp, "%s\n", curRoom);					
        	count++;
		}
    }while(curRoom[0] != '!');
		
    fclose(fp);

    printf("\nYOU HAVE FOUND THE END ROOM.\nCONGRATULATIONS!\nYOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", count);
    
    int character;                                      //Print out the order from the order temp file
    fp = fopen(order, "r");
    if (fp) {
        character = getc(fp);
        while (character != EOF){
            putchar(character);
            character = getc(fp);
        }
        fclose(fp);
    }
    
    remove(order);										//Remove the order temp file
    exit(0);
}


char *moveLocation(char *curRoom){
    char dirName[80];
    strcpy(dirName, "maillarw.rooms.");
    sprintf(dirName + strlen(dirName), "%ld/%s", (long) getpid(), curRoom);  //Copy the path to the current room into dirName

    char buff[200];
    char connections[256];

	FILE *fp = fopen(dirName, "r");											//Open the current room's file for reading
    fgets(buff, 200, fp);

    int i = 0;                                       
    while(buff[5] != 'T'){													//Get all the rooms that are connected to the current room
		if(buff[0] == 'C'){													//by using the descriptions that prefix them
			char *connection = getLastWord(buff);							//Place them in connections
			if(i <= 1){
				strcpy(connections, connection);
			}
			else{
				sprintf(connections + strlen(connections), ", %s", connection);
			}
			i++;
		}
		fgets(buff, 200, fp);
    }
    sprintf(connections + strlen(connections), ".");

    
    char *position = getLastWord(buff);										//Get the position of the current room

    fclose(fp);

    if(!strcmp("END_ROOM", position)){              						//If the position is the end, return !
        static char theEnd[256];
        sprintf(theEnd + strlen(theEnd), "!"); 
        return theEnd;
    }
    
    int found = 0;                                  						//Loop until a valid room is entered
    while(found == 0){
        static char location[256];
        strcpy(location, userInterface(curRoom, connections));				//userInterface asks for a room and returns the response
        
        if(strstr(connections, location) != NULL){							//Checks to see the room is valid by comparing it to connections
            return location;
        }
        else printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
    }

}

char *getLastWord(char *buff){												//Gets the last word in a line that is formatted in the way 
    static char theWord[256];												//that they are in this program (two spaces on each line)
    int spaces = 0;
    int j = 0;
    while(spaces < 2){														//Loops until two spaces are found
		if(buff[j] == ' '){
			spaces++;
		}
		j++;
	}
	
	int k = 0;
	do{																		//Gets the length of the new word (k)
		theWord[k] = buff[j];
		j++;
		k++;
	} while(buff[j] != '\n');
    theWord[k] = '\0';														//Terminates the word with \0				
    return theWord;
}

char *userInterface(char *curRoom, char *connections){						//The user interface for getting room names
    printf("\nCURRENT LOCATION: %s\n", curRoom);
    printf("POSSIBLE CONNECTIONS: %s\n", connections);
    printf("WHERE TO?>");

    static char location [256];
    fgets(location, 256, stdin);
    location[strcspn(location, "\n")] = 0;
    
    return location;														//Returns the user's response

}


char *createFiles(){
    srand(time(NULL));

    																		//Make the directory for the rooms
    char dirName[80];
    strcpy(dirName, "maillarw.rooms.");
    sprintf(dirName + strlen(dirName), "%ld", (long) getpid());
    mkdir(dirName, S_IRWXU);

    																		//usedRooms will hold 1 if the room is to be use and 0 otherwise
    int usedRooms[10] = {0}; 
    int numChosen = 0;

	while(numChosen < 7){													//Randomly put 1's in 7 separate room numbers (out of 10)		
        int room = rand() % 10;
        if(usedRooms[room] == 0){
            usedRooms[room] = 1;
            numChosen++;
        }
    }

    int theRooms[7];														//theRooms holds the 7 room numbers
    int i = 0;
    int j = 0;
    for(i; i < 10; i++){													//Fill theRooms based on the 1's in usedRooms
        if(usedRooms[i] == 1){
            theRooms[j] = i;
            j++;
        }
    }

    int start = theRooms[rand() % 7];										//Randomly choose start room
    int end;
    do{																		//Randomly choose an end room that is different than the start
        end = theRooms[rand() % 7];
    } while(end == start);

    int name;
    i = 0;
    int section = 1; 														//Sections: start = 0, mid = 1, end = 2

    char *names[10] = {"Dungeon", "Garden", "Ball_Room", "Chamber", "Secret_Passage", "Library", "Dining_Room",
                        "Kitchen", "Parlour", "Games_Room"};

    for(i; i < 7; i++){														//Determine what section each room is in...
        if(theRooms[i] == start){
            section = 0;
        }
        else if (theRooms[i] == end){
            section = 2;
        }
        else section = 1;

        name = theRooms[i];
        makeRoom(dirName, theRooms, section, name);							//And sent it, the directory name, the room name, and the list of
																			//rooms to makeRoom, to make the room file.
        }

        return names[start];												//Return the name of the starting room



}
char *getDirectory(){														//Get the directory name, returns the temp directory name.
    static char dirName[256];
    strcpy(dirName, "maillarw.rooms.");
    sprintf(dirName + strlen(dirName), "%ld", (long) getpid());
    return dirName;
}


void makeRoom (char dirName[80], int theRooms[7], int section, int name){

    char *names[10] = {"Dungeon", "Garden", "Ball_Room", "Chamber", "Secret_Passage", "Library", "Dining_Room",
		"Kitchen", "Parlour", "Games_Room"};								//The list of all ten possible rooms

    char newFile[200];
    sprintf(newFile, "%s/%s", dirName, names[name]);						//Create a new room file in the room directory...

    FILE *fp;

    fp = fopen(newFile, "w+");
    fprintf(fp, "ROOM NAME: %s\n", names[name]);							//Open the file for writing and write the room name in it

    int connections[7] = {0};
    int numConnections = rand() % 4 + 3;									//Randomly choose how many connections the room will have (3-6)
    int i = 1;
    int conn;

    for(i; i <= numConnections; i++){										//Determine each connection and write each connection to file
        int found = 0;
        do{
            conn = rand() % 7;												//Randomly choose a connection and use it if it hasn't been used 
            if(connections[conn] == 0 && theRooms[conn] != name){			//already by this room
                fprintf(fp, "CONNECTION %i: %s\n", i, names[theRooms[conn]]);
                connections[conn] = -1;
                found = 1;
            }

        } while(found == 0);
    }

    char sectionName[20] = "";
    if(section == 0){														//Create a string depending on the section number
        sprintf(sectionName + strlen(sectionName), "START_ROOM");
    }
    else if(section == 2){
        sprintf(sectionName + strlen(sectionName), "END_ROOM");
    }
    else sprintf(sectionName + strlen(sectionName), "MID_ROOM");

    fprintf(fp, "ROOM TYPE: %s\n", sectionName);							//Write out the room type to file

    fclose(fp);


}
