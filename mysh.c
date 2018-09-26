#include "functions.h"

int main(int argc, char *argv[], char *envp[]){

    char lineCpy[128];
    

    while (1){

        //Prompt user for a command
        printf("Command: ");
        fflush(stdout);

        fgets(line, 128, stdin);
        
        //chop the tail off the line, set to null
        line[strlen(line) - 1] = 0;

        strcpy(lineCpy, line);

        //results stored in global args array
        tokenize(lineCpy);

        executeCommand();
        
        cleanup();
    }

    return 0;
}