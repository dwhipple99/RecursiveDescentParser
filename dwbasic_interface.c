
/* This header file includes all the global #defines required as well as function prototypes. */

//*****************************************************************************************************
//
//  The main functions required to provide a general user interface to dwbasic are contained within 
//  this file.
//
//*****************************************************************************************************

#include "dwbasic.h"

void do_cls()
{
   system("clear");
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - welcome_screen                                          **/
/**                                                                    **/
/** PURPOSE  - The purpose of the function is to present a menu for    **/
/**            times that the interpreter is run interactively.        **/
/**                                                                    **/
/**            This is basically the main menu.                        **/
/**                                                                    **/
/** INPUT PARMS - None                                                 **/
/**                                                                    **/
/** RETURNS     - Returns 0 always                                     **/
/**                                                                    **/
/************************************************************************/
int welcome_screen(char *program_name, char *program, char *prog)
{
   int i, firstrun=1;
   char choice=0;
   float version=0.1;

   int memory=MAX_PROGRAM_SIZE, program_size=0;

   if (NULL == prog) {
      error(10);  /* Couldn't allocate the memory */
   }
   else {
      system("clear");
      printf("Welcome to DW-BASIC (Version-%f).\n", version);
      printf("%d bytes of memory free for program.\n\n", memory);
      do {
            if (choice != 10) {
               if (strlen(program_name) == 0)
                  printf("\n1. Load a program     (NO PROGRAM CURRENTLY LOADED)\n\n");
               else
               {
                  printf("\n\n1. Load a program     (%s, size=%d bytes)\n", program_name, program_size);
                  printf("2. List the program \n");
                  printf("3. Run the program \n");
               }


               printf("6. Exit to shell \n\n\n");

            }

            if (choice == 10 || firstrun == 1)
            {
               printf("--> ");
               firstrun = 0;
            }

            choice = getchar();

            if (choice == '1')
            {
               printf("Enter Filename -> ");
               scanf("%s", program_name);
               program_size = load(program_name, prog);
               if (program_size == 0)
               {
                  *program_name = 0;
                  error(1);
               }
               else
                  program = prog;
            }
            else {
              if (choice == '2')
               {
                  list(program);
                  program = prog;
               }
               else {
                  if (choice == '3')
                  {
                  }
                  else {
                     if (choice == '6')
                     {
                        free_memory();
                        exit(0);
                     }
                  }
               }
            }
      } while (choice != '6');
   }
   return 0;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - load                                                    **/
/**                                                                    **/
/** PURPOSE  - The purpose of this function is to load a program from  **/
/**            file on disk (filename previously known) into a         **/
/**            memory buffer.                                          **/
/**                                                                    **/
/** INPUT PARMS -[char *filename]- This parameter is the filename to   **/
/**                                load into memory.                   **/
/**                                                                    **/
/**              [char *prog]- This paramater is the memory buffer to  **/
/**                            to load the file into.                  **/
/**                                                                    **/
/** RETURNS     - Returns 1 always right now, eventually should        **/
/**               handle errors.                                       **/
/**                                                                    **/
/************************************************************************/
int load(char *filename, char *prog)
{
   FILE *fp;
   unsigned int i=0;
   char *p1;
   int choice=0;

   printf("Loading program \"%s\".\n", filename);

   if (!(fp=fopen(filename, "rb")))
      return 0;
   i=0;
   p1 = prog;
   do {
      *p1 = getc(fp);
      p1++;
      i++;
      printf("#");
   } while ((!feof(fp)) && (i<MAX_PROGRAM_SIZE));

   if (i == MAX_PROGRAM_SIZE)
      error(12);

   --i;
   *(p1-2) = 0;
   fclose(fp);
   printf("\nLoaded %d characters.\n", i);
   //scanf("%d", &choice);
   return i;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - list                                                    **/
/**                                                                    **/
/** PURPOSE  - The purpose of this function is to list the current     **/
/**            program in memory to the screen for the user to see.    **/
/**                                                                    **/
/** INPUT PARMS -[char *program]- This Parameter is the pointer to the **/
/**                               program to list.                     **/
/**                                                                    **/
/** RETURNS     - Returns NOTHING (Void)                               **/
/**                                                                    **/
/************************************************************************/
void list(char *program)
{
   int choice;
   // system("clear");
   printf("***** PROGRAM LISTING *****\n");
   while (!(*program == 0))
   {
      printf("%c", *program);
      program++;
   }
   printf("\n*** END PROGRAM LISTING ***\n");
   //printf("\nPress any key to continue.");
   //scanf("%d", &choice);
}

