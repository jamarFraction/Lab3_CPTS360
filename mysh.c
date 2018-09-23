#include "functions.h"

int main(void){

    char lineCpy[128];
    

    while (1){

        //Prompt user for a command
        printf("Command: ");
        fflush(stdout);

        fgets(line, 128, stdin);
        
        //chop the tail off the line, set to null
        line[strlen(line) - 1] = 0;

        strcpy(lineCpy, line);

        //resuls stored in global args array
        tokenize(lineCpy);

        //per assignment instructions, special case commands
        if (strcmp(args[0], "exit") == 0){
            //outta here
            exit(1);
        }else if(strcmp(args[0], "cd") == 0){

            if (args[1] != 0 && changeDir() == 0){

                //success message
                printf("cd to %s successful\n", args[1]);
            }else{

                printf("cd to %s failed\n", args[1]);
            }
        }




    }
    return 0;
}