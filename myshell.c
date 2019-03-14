/****************************************************************
 * Name        :  Cory Lewis                                    *
 * Class       :  CSC 415                                       *
 * Date        :  2/27/19                                       *
 * Description :  Writting a simple bash shell program          *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)

//Helper Structs
struct Redirection{
   int isRedirection;
   int fd;
};

//Prototypes
void tokenizeInput(int* _isRedirection, int* _fd);
int isRedirection(char x[]);
int openFileWrite(char fileName[]);
int openFileWriteAppend(char fileName[]);
int openFileRead(char fileName[]);



//Globals
char * myargv[BUFFERSIZE];
char *token;
char inputString[BUFFERSIZE];
int myargc;

/*
Document: struct Redirection is used to keep track if a file is being read from or written into
*/

int main(int* argc, char** argv){
   //Initialize
   struct Redirection redirection;
   redirection.isRedirection = 0;
   redirection.fd = -1;

   printf(PROMPT);
   tokenizeInput(&redirection.isRedirection, &redirection.fd);
 

   execvp(myargv[0],myargv);

   //Close redirection file
   if(redirection.isRedirection){
      close(redirection.fd);
      redirection.isRedirection = 0;
      redirection.fd = -1;
   }
    
return 0;
}
void tokenizeInput(int* _isRedirection, int * _fd){
   myargc = 0;
   //Declare
   const char delimiter[4] = " ";
   
   //Input
   scanf("%[^\n]", inputString);
   
   //Initialize Token
   token = strtok(inputString, delimiter);

   //Tokenize Array
   //int counter = 0;
   while(token != 0){
      myargv[myargc] = token;
      if(isRedirection(myargv[myargc])){
         *_isRedirection = 1;
         if(!strcmp(myargv[myargc],">")){
         //Write to file
         *_fd = openFileWrite(strtok(0, delimiter));
         dup2(*_fd, fileno(stdout));
         }else if(!strcmp(myargv[myargc],">>")){
         //Append to file
         *_fd = openFileWriteAppend(strtok(0, delimiter));
         dup2(*_fd, fileno(stdout));
         }else if(!strcmp(myargv[myargc],"<")){
         //write into file
         *_fd = openFileRead(strtok(0, delimiter));
         dup2(*_fd, fileno(stdin));
         }
      }else{
         myargc++;
      }
      token = strtok(0, delimiter);
   }
   myargv[myargc] = NULL;
}
int isRedirection(char x[]){
   return (!strcmp(myargv[myargc],">") || !strcmp(myargv[myargc],">>") || !strcmp(myargv[myargc],"<"));
}
int openFileWrite(char fileName[]){
   int fd;
   fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU);
    if(fd < 0){
        perror("[openFileWriteTruncate]Destination file could not be opened/created\n");
        return -42;
    }
    return fd;
}
int openFileWriteAppend(char fileName[]){
   int fd;
   fd = open(fileName, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    if(fd < 0){
        perror("[openFileWriteAppend]Destination file could not be opened/created\n");
        return -42;
    }
    return fd;
}
int openFileRead(char fileName[]){
   int fd;
   fd = open(fileName, O_RDONLY);
   if(fd < 0){
      perror("[openFileRead]Destination file could not be opened\n");
      return -42;
   }
   return fd;
}
