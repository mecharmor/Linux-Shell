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


/*
	Citing Sources:
	//Used this for learning strtok -> https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
	
*/

int 
main(int* argc, char** argv)
{

   char inputString[BUFFERSIZE];
   const char delimiter[4] = " ";
   char *token;

   /* Prompt, scan until line is read */
   printf(PROMPT);
   scanf("%[^\n]", inputString);

   /* first token pointer */
   token = strtok(inputString, delimiter);
   
   /* loop through tokens */
   while( token != 0 ) {
      printf( " %s\n", token);
    
      token = strtok(0, delimiter);
   }
    
return 0;
}
