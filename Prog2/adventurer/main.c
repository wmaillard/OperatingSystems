#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char *createFiles();
void sayCurDir(); //for debugging
void makeRoom (char dirName[80], int theRooms[7], int section, int name);
char *userInterface(char *curRoom, char *connections);
char *moveLocation(char *curRoom);
char *getLastWord(char *buff);
char *getDirectory();

int main()
{
    printf("Hello world!\n");
    int count = 0;

    char *curRoom = createFiles();
    
    char order[256];
    sprintf(order, "%s/tempPathFile", getDirectory());

    FILE *fp;
    fp = fopen(order, "w+");

    while(curRoom[0] != '!'){
        fprintf(fp, "%s\n", curRoom);
        count++;
        curRoom = moveLocation(curRoom);
    }
    fclose(fp);

    printf("\nYOU HAVE FOUND THE END ROOM.\nCONGRATULATIONS!\nYOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS:\n", count);
    
    int character;                                      //Print out the order
    fp = fopen(order, "r");
    if (fp) {
        character = getc(fp);
        while (character != EOF){
            putchar(character);
            character = getc(fp);
        }
        fclose(fp);
    }
    
    remove(order);
    exit(0);
}


char *moveLocation(char *curRoom){
    char dirName[80];
    strcpy(dirName, "maillarw.rooms.");
    sprintf(dirName + strlen(dirName), "%ld/%s", (long) getpid(), curRoom);

    char buff[200];
    char connections[256];

    FILE *fp = fopen(dirName, "r");
    fgets(buff, 200, fp);

    int i = 0;                                       //Get the connecting rooms
    while(buff[5] != 'T'){

    if(buff[0] == 'C'){
        char *connection = getLastWord(buff);
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

    //Get the position
    char *position = getLastWord(buff);

    fclose(fp);

    if(!strcmp("END_ROOM", position)){              //Check if position is the end
        static char theEnd[256];
        sprintf(theEnd + strlen(theEnd), "!%s", curRoom); //need this or just return !?

        return theEnd;
    }
    
    int found = 0;                                  //Verify user's choice
    while(!found){
        static char location[256];
        strcpy(location, userInterface(curRoom, connections));
        
       /* printf("CURRENT LOCATION: %s\n", curRoom);
        printf("POSSIBLE CONNECTIONS: %s\n", connections);
        printf("WHERE TO?>");
        
        static char location [256];
        fgets(location, 256, stdin);
        location[strcspn(location, "\n")] = 0;*/
        
        
        
        
        if(strstr(connections, location) != NULL){
            return location;
        }
        else printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
    }

}

char *getLastWord(char *buff){
    static char theWord[256];
    int spaces = 0;
    int j = 0;
    while(spaces < 2){
            if(buff[j] == ' '){
                spaces++;
            }
            j++;
        }
        int k = 0;
        do{
            theWord[k] = buff[j];
            j++;
            k++;
        } while(buff[j] != '\n');
    theWord[k] = '\0';
    return theWord;
}

char *userInterface(char *curRoom, char *connections){
    printf("\nCURRENT LOCATION: %s\n", curRoom);
    printf("POSSIBLE CONNECTIONS: %s\n", connections);
    printf("WHERE TO?>");

    static char location [256];
    fgets(location, 256, stdin);
    location[strcspn(location, "\n")] = 0;
    
    return location;

}


char *createFiles(){
    srand(time(NULL));

    //***Make directory***
    char dirName[80];
    strcpy(dirName, "maillarw.rooms.");
    sprintf(dirName + strlen(dirName), "%ld", (long) getpid());
    mkdir(dirName, S_IRWXU);

    //***Choose what rooms***
    int usedRooms[10] = {0}; //fill up what rooms to use first then move the rooms to theRooms
    int numChosen = 0;

    while(numChosen < 7){
        int room = rand() % 10;
        if(usedRooms[room] == 0){
            usedRooms[room] = 1;
            numChosen++;
        }
    }

    int theRooms[7];
    int i = 0;
    int j = 0;
    for(i; i < 10; i++){
        if(usedRooms[i] == 1){
            theRooms[j] = i;
            j++;
            printf("room #: %i\n", i);
        }
    }

    int start = theRooms[rand() % 7];
    int end;
    do{
        end = theRooms[rand() % 7];
    } while(end == start);

    printf("start: %i, end: %i\n", start, end);

    int name;
    i = 0;
    int section = 1; //start = 0, mid = 1, end = 2

    char *names[10] = {"Dungeon", "Garden", "Ball_Room", "Chamber", "Secret_Passage", "Library", "Dining_Room",
                        "Kitchen", "Parlour", "Games_Room"};

    for(i; i < 7; i++){
        if(theRooms[i] == start){
            section = 0;
        }
        else if (theRooms[i] == end){
            section = 2;
        }
        else section = 1;

        name = theRooms[i];
        makeRoom(dirName, theRooms, section, name);

        }


        return names[start];



}
char *getDirectory(){
    static char dirName[256];
    strcpy(dirName, "maillarw.rooms.");
    sprintf(dirName + strlen(dirName), "%ld", (long) getpid());
    return dirName;
}


void makeRoom (char dirName[80], int theRooms[7], int section, int name){

    char *names[10] = {"Dungeon", "Garden", "Ball_Room", "Chamber", "Secret_Passage", "Library", "Dining_Room",
                        "Kitchen", "Parlour", "Games_Room"};

    char newFile[200];
    sprintf(newFile, "%s/%s", dirName, names[name]);

    FILE *fp;

    fp = fopen(newFile, "w+");
    fprintf(fp, "ROOM NAME: %s\n", names[name]);

    int connections[7] = {0};
    int numConnections = rand() % 4 + 3;
    int i = 1;
    int conn;

    for(i; i <= numConnections; i++){
        int found = 0;
        do{
            conn = rand() % 7;
            if(connections[conn] == 0 && theRooms[conn] != name){
                fprintf(fp, "CONNECTION %i: %s\n", i, names[theRooms[conn]]);
                connections[conn] = -1;
                found = 1;
            }

        } while(found == 0);
    }

    char sectionName[20] = "";
    if(section == 0){
        sprintf(sectionName + strlen(sectionName), "START_ROOM");
    }
    else if(section == 2){
        sprintf(sectionName + strlen(sectionName), "END_ROOM");
    }
    else sprintf(sectionName + strlen(sectionName), "MID_ROOM");

    fprintf(fp, "ROOM TYPE: %s\n", sectionName);

    fclose(fp);


}

void sayCurDir(){
char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
   else
       perror("getcwd() error");
   return 0;
}
