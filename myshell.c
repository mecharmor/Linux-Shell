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
struct CD{
   bool isCD;
   char * path;
};
//Prototypes
void tokenizeInput(struct Redirection * redir);
int isRedirection(char * token);
int openFileWrite(char fileName[]);
int openFileWriteAppend(char fileName[]);
int openFileRead(char fileName[]);
void resetGlobals();
char * newPrompt();
int getDirectories();

//Globals
char * myargv[BUFFERSIZE];
char *token;
int myargc;
bool waitForChildProcess;
struct CD cd;
bool isPWD;

//Piping Globals
char * myargv2[BUFFERSIZE];
int myargc2;
bool isPiping;

int main(int* argc, char** argv){
   //Initialize
   struct Redirection redirection;
   redirection.isRedirection = false;
   redirection.fd = -1;
   redirection.isOutput = false;
   redirection.appendOutput = false;
   myargc = 0;
   waitForChildProcess = true;
   cd.isCD = false;
   cd.path = "";
   isPWD = false;
   myargc2 = 0;
   isPiping = false;


   while(1){
      printf("%s" ,newPrompt());

      //Get Input and tokenize
      tokenizeInput(&redirection);

      pid_t pid;
      pid = fork();
      
      if(pid > 0){//parent

         if(cd.isCD){
            if(chdir(cd.path) < 0){
               printf("Could not find directory: %s\n", cd.path);
            }
         }else if(isPWD){
            printf("%s\n", getcwd(NULL, BUFFERSIZE));
         }

         if(waitForChildProcess){
            waitpid(pid, 0, 0);
         }
      }else if(pid == 0){//child
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
         //Time for piping
         if(isPiping){
            int pipe_fd[2]; // [0] read end, [1] write end
            if(pipe(pipe_fd) < 0){
               perror("Pipe failed to create \n");
            }

            int id = fork();
            //Child, right side
            if(id == 0){
               close(0);
               dup(pipe_fd[0]);
               close(pipe_fd[0]);
               close(pipe_fd[1]);

               execvp(myargv2[0],myargv2);
               perror("pipe right side failed\n");
            }
            //Parent, left side
            if(id > 0){
               close(1);
               dup(pipe_fd[1]);
               close(pipe_fd[0]);
               close(pipe_fd[1]);
               execvp(myargv[0],myargv);
               perror("pipe left side failed\n");
            }
         }else{
            execvp(myargv[0],myargv);
            perror("exec failed");
         }

      }else{
         perror("forking failed");
      }
         resetGlobals();
         //Reset Redirection variables
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
   cd.isCD = false;
   cd.path = "";
   isPWD = false;

   //Reset Piping values
   memset(myargv2, 0, BUFFERSIZE);
   myargc2 = 0;
   isPiping = false;
}
char * newPrompt(){
   char * pwd = getcwd(NULL, BUFFERSIZE);
   char * _prompt = malloc(PROMPTSIZE + strlen(pwd)+1024);
   char * _token;

   if(getDirectories() > 2){
      _token = strtok(pwd, "/");//ignore home
      _token = strtok(0, "/");//ignore profile
      _token = strtok(0, "/");//set next token
   
      strcat(_prompt,"myshell ~/");
       while(_token != NULL){
          strcat(_prompt, _token);
          strcat(_prompt,"/");
          _token = strtok(0, "/");
       }
   }else{
      strcat(_prompt,"myshell ");
      strcat(_prompt, pwd);
   }
   strcat(_prompt,">> ");
   return strdup(_prompt);
}
int getDirectories(){
   char * str = getcwd(NULL, BUFFERSIZE);
   char * _token = strtok(str, "/");
   int counter = 0;

   while(_token != NULL){
      _token = strtok(NULL, "/");
      counter++;
   }
   return counter;
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
   while(token != NULL){
      if(isRedirection(token)){
         redir->isRedirection = true;
         if(!strcmp(token,">")){
         //Write to file
         redir->isOutput = true;
         redir->appendOutput = false;
         }else if(!strcmp(token,">>")){
         //Append to file
         redir->isOutput = true;
         redir->appendOutput = true;
         }else if(!strcmp(token,"<")){
         //write into file
         redir->isOutput = false;
         }
         //Deep Copy, File Name
         redir->fileName = strdup(strtok(0, delimiter));
      }else if(!strcmp(token,"&")){
         waitForChildProcess = false;
      }else if(!strcmp(token,"cd")){
         cd.isCD = true;
         cd.path = strdup(strtok(0, delimiter));
      }else if(!strcmp(token,"pwd")){
         isPWD = true;
      }else if(!strcmp(token,"|")){
         isPiping = true;
      }else{
         if(isPiping){
            myargv2[myargc2] = strdup(token);
            myargc2++; 
         }else{
            myargv[myargc] = strdup(token);
            myargc++;
         }
      }
      token = strtok(0, delimiter);
   }
   myargv[myargc] = NULL;
   if(isPiping){
      myargv2[myargc2] = NULL;
   }
}
int isRedirection(char * token){
   return (!strcmp(token,">") || !strcmp(token,">>") || !strcmp(token,"<"));
}
int openFileWrite(char fileName[]){
   int fd;
   fd = open(fileName, O_WRONLY | O_CREAT, S_IRWXU);
    if(fd < 0){
        perror("[openFileWrite] Destination file could not be opened/created\n");
        return -1;
    }
    return fd;
}
int openFileWriteAppend(char fileName[]){
   int fd;
   fd = open(fileName, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    if(fd < 0){
        perror("[openFileWriteAppend] Destination file could not be opened/created\n");
        return -1;
    }
    return fd;
}
int openFileRead(char fileName[]){
   int fd;
   fd = open(fileName, O_RDONLY);
   if(fd < 0){
      perror("[openFileRead] Destination file could not be opened\n");
      return -1;
   }
   return fd;
}
