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
        if (strcmp(args[0], "exit") == 0){
            //outta here
            exit(1);
        }else if(strcmp(args[0], "cd") == 0){

            //check for successful cd 
            if (changeDir() == 0){
                
                printf("cd to %s successful\n", (strcmp(args[1], "\0") == 0) ? "$HOME": args[1]);                
            }else{
                //bruh, it be like that sometimes..
                printf("cd to %s failed\n", args[1]);
            }

            //testing
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
                
                //testing
                char *myargv[16];

                //create the myargv array from the existing args
                createMyargv(myargv);

                //redirection check
                if (isRedirect(myargv) != -1)
                {
                    //redirect points have already been set
                    //execve will fail if it continues in the array after this point
                    myargv[myargc - 2] = NULL;
                }


                //execute the command
                //FIX SECOND ARGUMENT
                int success = execve(myargv[0], myargv, envp);
            
                //print out the error
                if (success == -1){

                     printf("%s\n", strerror(errno));
                }

                exit(0);
            }else{

                printf("Unable to create process\n");
            }
        }

        cleanup();
    }

    return 0;
}