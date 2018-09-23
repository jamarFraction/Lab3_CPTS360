#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

//array to hold the strings of arguments
//as an aside, i've been declaring these WRONG for years till now and never knew
char args[16][16];
char line[128]; //user input command line
char command[16], pathname[64];


void tokenize(char source[]){

    //temp string holder of strtok vals
    char *next = strtok(source, " ");

    int i = 0;

    //grab all space-separated user input
    //put in arg array
    while (next != 0)
    {

        strcpy(args[i], next);

        next = strtok(NULL, " ");

        i++;
    }
}

int changeDir(void){


    char *home;
    int success = -1;
    
    if(strcmp(args[1], "\0") == 0){

        home = getenv("HOME");

        success = chdir(home);

        return success;
    }

    //try to change the cwd to the path specified
    success = chdir(args[1]);

    return success;
}

//testing
//  char cwd[256];
// if (getcwd(cwd, sizeof(cwd)) != NULL)
// {
//     printf("Current working dir: %s\n", cwd);
// }
//  getcwd(cwd, sizeof(cwd));