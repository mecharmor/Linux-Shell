/****************************************************************
 * Name        :  Cory Lewis                                    *
 * Class       :  CSC 415                                       *
 * Date        :  3/14/19                                       *
 * Description :  Writting a simple bash shell program          *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/
/*
====== References, sources used for help in C =====
/Clearing the buffer: Source: https://www.geeksforgeeks.org/clearing-the-input-buffer-in-cc/
/Understanding strdup for deep copy: https://stackoverflow.com/questions/4090434/strtok-char-array-versus-char-pointer

*/
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)

//Helper Structs
struct Redirection{
   bool isRedirection; 
   int fd;
   bool isOutput;
   bool appendOutput;
   char * fileName;
};

//Prototypes
void tokenizeInput(struct Redirection * redir);
int isRedirection();
int openFileWrite(char fileName[]);
int openFileWriteAppend(char fileName[]);
int openFileRead(char fileName[]);
void resetGlobals();

//Globals
char * myargv[BUFFERSIZE];
char *token;
int myargc;
bool waitForChildProcess;

int main(int* argc, char** argv){
   //Initialize
   struct Redirection redirection;
   redirection.isRedirection = false;
   redirection.fd = -1;
   redirection.isOutput = false;
   redirection.appendOutput = false;
   myargc = 0;
   waitForChildProcess = true;

   while(1){

      printf(PROMPT);
      //Get Input and tokenize
      tokenizeInput(&redirection);

      //Fork
      pid_t pid;
      pid = fork();
      
      if(pid > 0){
         //parent
         if(waitForChildProcess){
            printf("parent waited\n");
            waitpid(pid, 0, 0);
         }
      }else if(pid == 0){
         //child
         if(redirection.isRedirection){
            if(redirection.isOutput){
               if(redirection.appendOutput){
                  // >> here
                  redirection.fd = openFileWriteAppend(redirection.fileName);
               }else{
                  // > here
                  redirection.fd = openFileWrite(redirection.fileName);
               }
               dup2(redirection.fd, 1);
               close(redirection.fd);
            }else{
               // < here
               redirection.fd = openFileRead(redirection.fileName);
               dup2(redirection.fd, 0);
               close(redirection.fd);
            }
         }
         execvp(myargv[0],myargv);
         perror("exec failed");
      }else{
         perror("forking failed");
      }
         resetGlobals();
         //Close redirection file
         if(redirection.isRedirection){
            redirection.isRedirection = false;
            redirection.isOutput = false;
            redirection.appendOutput = false;
            redirection.fd = -1;
         }
   }
return 0;
}
void resetGlobals(){
   memset(myargv, 0, BUFFERSIZE);
   token = NULL;
   myargc = 0;
   waitForChildProcess = true;
}
void tokenizeInput(struct Redirection * redir){
   char inputString[BUFFERSIZE];
   const char delimiter[4] = " ";
   char * returnValue[BUFFERSIZE];
   
   //Input
   scanf("%[^\n]", inputString);
   //Clear Buffer
   while ((getchar()) != '\n'); 
   
   //Get Token
   token = strtok(inputString, delimiter);

   //Tokenize Array
   while(token != 0){
      myargv[myargc] = token;
      if(isRedirection()){
         redir->isRedirection = true;
         if(!strcmp(myargv[myargc],">")){
         //Write to file
         redir->isOutput = true;
         redir->appendOutput = false;
         }else if(!strcmp(myargv[myargc],">>")){
         //Append to file
         redir->isOutput = true;
         redir->appendOutput = true;
         }else if(!strcmp(myargv[myargc],"<")){
         //write into file
         redir->isOutput = false;
         }
         //Deep Copy, File Name
         redir->fileName = strdup(strtok(0, delimiter));
      }else if(!strcmp(myargv[myargc],"&")){
         waitForChildProcess = false;
      }else{
         myargc++;
      }
      token = strtok(0, delimiter);
   }
   myargv[myargc] = NULL;
}
int isRedirection(){
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
