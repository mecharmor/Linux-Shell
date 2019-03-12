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

//Prototypes
void tokenizeInput();

//Globals
char * tokenizedArray[BUFFERSIZE];
char *token;
char inputString[BUFFERSIZE];

int main(int* argc, char** argv){
   printf(PROMPT);
   tokenizeInput();

   execvp(tokenizedArray[0],tokenizedArray);
    
return 0;
}

void tokenizeInput(){
   //Declare
   const char delimiter[4] = " ";
   
   //Input
   scanf("%[^\n]", inputString);
   
   //Initialize Token
   token = strtok(inputString, delimiter);

   //Tokenize Array
   int counter = 0;
   while(token != 0){
      tokenizedArray[counter] = token;
      counter++;
      token = strtok(0, delimiter);
   }
   tokenizedArray[counter] = NULL;
}