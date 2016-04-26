#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void createFiles();
void sayCurDir(); //for debugging
void makeRoom (char dirName[80], int theRooms[7], int section, char name[80]);

int main()
{
    printf("Hello world!\n");
    createFiles();
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

    char name[80];
    i = 0;
    int section = 1; //start = 0, mid = 1, end = 2
    for(i; i < 7; i++){
        if(i == start){
            section = 0;
        }
        else if (i == end){
            section = 2;
        }
        else section = 1;
        switch (i) {
        case 0: //make a file with x connections from theRooms where this may be the start or finish}
            sprintf(name, "Dungeon");
            makeRoom(dirName, theRooms, section, name);
            break;
















        }



    }
}

void makeRoom (char dirName[80], int theRooms[7], int section, char name[80]){
    printf("The name: %s\n", name);
    printf("The directory: %s\n", dirName);
}

void sayCurDir(){
char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
   else
       perror("getcwd() error");
   return 0;
}
