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

        
        //per assignment instructions, special case commands
        if (strcmp(userInput[0], "exit") == 0){
            //outta here
            exit(1);
        }else if(strcmp(userInput[0], "cd") == 0){

            //check for successful cd 
            if (changeDir() == 0){
                
                printf("cd to %s successful\n", (strcmp(userInput[1], "\0") == 0) ? "$HOME": userInput[1]);                
            }else{
                //bruh, it be like that sometimes..
                printf("cd to %s failed\n", userInput[1]);
            }

            //Print cwd info
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            printf("cwd is: %s\n", cwd);
        }else{

            //child:
            int pid, status;
            pid = fork();

            if (pid > 0)
            { // PARENT:
                printf("PARENT %d WAITS FOR CHILD %d TO DIE\n", getpid(), pid);
                pid = wait(&status);
                printf("DEAD CHILD=%d, HOW=%04x\n", pid, status);
            }
            else if(pid == 0)
            {
                
                //declare the variables that will hold the argv array and command path
                char commandPath[16] = "/bin/";

                //create the myargv array from the existing args,
                //concatenate the user's cmd to the path
                createMyargv(myargv);
                strcat(commandPath, myargv[0]);

                //redirection check
                if (isRedirect(myargv) != -1)
                {
                    //redirect points have already been set
                    //execve will fail if it continues in the array after this point
                    myargv[myargc - 2] = NULL;
                }

                //execute the command
                int success = execve(commandPath, myargv, envp);
            
                //print out the error in errno, if needed
                if (success == -1){

                     printf("%s\n", strerror(errno));
                }

                exit(0);
            }else{
                
                //failed process creation
                printf("Unable to create process\n");
            }
        }

        cleanup();
    }

    return 0;
}