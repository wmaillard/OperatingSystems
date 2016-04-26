#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void createFiles();
void sayCurDir(); //for debugging
void makeRoom (char dirName[80], int theRooms[7], int section, int name);

int main()
{
    printf("Hello world!\n");
    createFiles();
    return 0;
}

void createFiles(){
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




}

void makeRoom (char dirName[80], int theRooms[7], int section, int name){

    char *names[10] = {"Dungeon", "Garden", "Ball_Room", "Chamber", "Secret_Passage", "Library", "Dining_Room",
                        "Kitchen", "Parlour", "Games_Room"};

    printf("The name: %s\n", names[name]);
    printf("The directory: %s\n", dirName);

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
