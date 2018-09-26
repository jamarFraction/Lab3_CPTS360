#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

//array to hold the strings of arguments
//as an aside, i've been declaring these WRONG for years till now and never knew
char userInput[16][16];

//argv to be passed to execve
char *myargv[16];

//raw user line
char line[128];

int myargc = 0;

void tokenize(char source[])
{

    //temp string holder of strtok vals
    char *next = strtok(source, " ");

    int i = 0;

    //grab all space-separated user input
    //put in arg array
    while (next != 0)
    {

        strcpy(userInput[i], next);

        next = strtok(NULL, " ");

        i++;
        myargc += 1;
    }
}

int changeDir(void)
{

    char *home;
    int success = -1;

    if (strcmp(userInput[1], "\0") == 0)
    {

        home = getenv("HOME");

        success = chdir(home);

        return success;
    }

    //try to change the cwd to the path specified
    success = chdir(userInput[1]);

    return success;
}

void createMyargv(char *destination[])
{

    //create a useable args array with the absolute path of the binaries
    int i = 0;

    //copy over the values from the 2D user input to the 1D destination array
    while (userInput[i][0] != 0)
    {

        destination[i] = userInput[i];
        i++;
    }

    //set the null terminator pointer per argv needs
    destination[i] = NULL;
}

void cleanup()
{

    //clear the args from the past run
    for (int i = 0; i < 16; i++)
    {

        memset(userInput[i], 0, strlen(userInput[i]));
        //clear the myargv
    }

    //args count for the command must be reset
    myargc = 0;
}

int createApp(char *file)
{
    // close fd 1
    close(1);
    return open(file, O_WRONLY | O_APPEND, 0644);
}

int createOut(char *file)
{
    // close fd 1
    close(1);

    return open(file, O_WRONLY | O_CREAT, 0644);
}

int createIn(char *file)
{
    // close fd 0
    close(0);

    return open(file, O_RDONLY);
}

int redirect(int direction, char filename[])
{
    if (direction == 1)
    {
        return createIn(filename);
    }
    else if (direction == 2)
    {
        return createOut(filename);
    }
    else if (direction == 3)
    {
        return createApp(filename);
    }
    return 1;
}

int isRedirect(char *argv[])
{

    int i = 0;
    char temp[16];

    while (argv[i])
    {
        strncpy(temp, argv[i], 16);

        //char *temp = argv[i];

        if (strcmp(temp, "<") == 0)
        { // in

            strncpy(temp, argv[i + 1], 16);
            redirect(1, temp);

            return 1;
        }
        else if (strcmp(temp, ">") == 0)
        { // out

            strncpy(temp, argv[i + 1], 16);
            redirect(2, temp);
            return 2;
        }
        else if (strcmp(temp, ">>") == 0)
        { // append

            strncpy(temp, argv[i + 1], 16);
            redirect(3, temp);
            return 3;
        }
        i++;
    }
    return -1;
}

int forkChild()
{

    //child:
    int pid, status;

    

    pid = fork();

    if (pid > 0)
    { // PARENT:
        printf("PARENT %d WAITS FOR CHILD %d TO DIE\n", getpid(), pid);
        pid = wait(&status);
        printf("DEAD CHILD=%d, HOW=%04x\n", pid, status);
    }
    else if (pid == 0)
    {

        //declare the variables that will hold the argv array and command path
        char commandPath[16] = "/bin/";

        //create the myargv array from the existing args,
        //concatenate the user's cmd to the path
        createMyargv(myargv);
        strcat(commandPath, myargv[0]);

        char head[16], tail[16];

        // if (isPipe(head, tail))
        // {
        //     printf("Deez\n");
        // }
        //else
        if (isRedirect(myargv) != -1) //redirection check
        {
            //redirect points have already been set
            //execve will fail if it continues in the array after this point
            myargv[myargc - 2] = NULL;
        }
        //execute the command
        int success = execve(commandPath, myargv, NULL);

        if (success == -1)
        {
            //print out the error in errno, if needed
            printf("%s\n", strerror(errno));
        }

        exit(0);

        
    }else{//failed process creation

        printf("Unable to create process\n");
    }
}

int isPipe(char head[], char tail[])
{

    //hoop through the
    return 0;
}