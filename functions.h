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
char userInput[16][16];

//argv to be passed to execve
char *ARGV[16];

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


int isPipe(char *head[], char *tail[])
{

    int i = 0;
    while (ARGV[i])
    {
        if (strcmp(ARGV[i], "|") == 0)
        { // it has pipe
            //assign head and tail
            int j = 0;
            while (ARGV[j])
            {
                if (j < i)
                { // head
                    head[j] = ARGV[j];
                }
                else if (j > i)
                { // tail
                    tail[j - 3] = ARGV[j];
                }
                j++;
            }
            return 1;
        }
        i++;
    }
    return 0;
}

int execute(char *myargv[]){

    //declare the variables that will hold the argv array and command path
    char commandPath[16] = "/bin/";
    
    //concatenate the user's cmd to the path
    strcat(commandPath, myargv[0]);

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
}

void connectPipe(char *head[], char *tail[]){

    int pd[2], pid;

    pipe(pd); // creates a PIPE; pd[0] for READ  from the pipe,
              //                 pd[1] for WRITE to   the pipe.

    pid = fork(); // fork a child process
                  // child also has the same pd[0] and pd[1]

    if (pid)
    {                 // parent as pipe pipe WRITER
        close(pd[0]); // WRITER MUST close pd[0]

        close(1);     // close 1
        dup(pd[1]);   // replace 1 with pd[1]
        close(pd[1]); // close pd[1] since it has replaced 1
        execute(head);   // change image to cmd1
    }
    else
    {                 // child as pipe pipe READER
        close(pd[1]); // READER MUST close pd[1]

        close(0);
        dup(pd[0]);   // replace 0 with pd[0]
        close(pd[0]); // close pd[0] since it has replaced 0
        execute(tail);   // change image to cmd2
    }
}

int forkChild()
{

    int pid, status;

    pid = fork();

    if (pid > 0) //parent
    {            
        printf("PARENT %d WAITS FOR CHILD %d TO DIE\n", getpid(), pid);
        pid = wait(&status);
        printf("DEAD CHILD=%d, HOW=%04x\n", pid, status);
    }
    else if (pid == 0) //child:
    {
        
        char *head[16], *tail[16];

        if (isPipe(head, tail) == 1)
        {
            connectPipe(head, tail);
        }else{

            execute(ARGV);
        }

        exit(0);

    }
    else //failed process creation
    { 
        printf("Unable to create process\n");
    }
}

void executeCommand(void){

    //create the myargv array from the existing args,
    createMyargv(ARGV);

    //per assignment instructions, special case commands
    if (strcmp(userInput[0], "exit") == 0)
    {
        //outta here
        exit(1);
    }
    else if (strcmp(userInput[0], "cd") == 0)
    {

        //check for successful cd
        if (changeDir() == 0)
        {

            printf("cd to %s successful\n", (strcmp(userInput[1], "\0") == 0) ? "$HOME" : userInput[1]);
        }
        else
        {
            //bruh, it be like that sometimes..
            printf("cd to %s failed\n", userInput[1]);
        }

        //Print cwd info
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        printf("cwd is: %s\n", cwd);
    }
    else
    {
        forkChild();
    }
}
