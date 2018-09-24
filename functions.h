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
char args[16][16];
char line[128]; //user input command line
//char head[16], tail[16];
int myargc = 0; 

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
        myargc += 1;
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

void createMyargv(char *destination[]){

    //create a useable args array with the absolute path of the binaries
    int i = 1;
    char path[32] = "/bin/";
    strcat(path, args[0]);
    destination[0] = path;
    
    //copy over the remainder of the args
    while (args[i][0] != 0)
    {

        destination[i] = args[i];
        i++;
    }

    //set the null terminator pointer per argv needs
    destination[i] = NULL;
}

void cleanup(){

    //clear the args from the past run
    for(int i = 0; i < 16; i++){

        memset(args[i],0,strlen(args[i]));

    }

    //args count for the command must be reset
    myargc = 0;
}

int createApp(char *file)
{
  // close fd 1
  close(1);
  return open(file, O_WRONLY|O_APPEND, 0644);
}

int createOut(char *file)
{
  // close fd 1
  close(1);

  return open(file, O_WRONLY|O_CREAT, 0644);
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
  else if(direction == 2)
  {
    return createOut(filename);
  }
  else if(direction == 3)
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
//testing
//  char cwd[256];
// if (getcwd(cwd, sizeof(cwd)) != NULL)
// {
//     printf("Current working dir: %s\n", cwd);
// }
//  getcwd(cwd, sizeof(cwd));