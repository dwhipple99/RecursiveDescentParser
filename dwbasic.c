/*

   PROGRAM - dwbasic.c

   DATE    - 12/30/2012 (Started redevelopment of old college parser from 2/28/92

   PURPOSE - To implement a simple recursive descent parser that can be leveraged
             across multiple projects.

*/

/* HEADER FILES */
#include "dwbasic.h"

//    LET: assigns a value (which may be the result of an expression) to a variable.
//    DATA: holds a list of values which are assigned sequentially using the READ command.
//  GOTO: jumps to a numbered or labelled line in the program.
// GOSUB: temporarily jumps to a numbered or labelled line, returning to the following line after encountering the RETURN Command. This is used to implement subroutines.
// INPUT: asks the user to enter the value of a variable. The statement may include a prompt message.
// FOR ... TO ... {STEP} ... NEXT: repeat a section of code a given number of times. A variable that acts as a counter is available within the loop.
// COMMAND TO ADD
//   WAIT wait x seconds
//  DIM for ARRAYS
// SUB for SUB ROUTINES ??
// REGULAR EXPRESSION PARSING??
// XML LIBRARY PARSEING???

struct commands {
   char command[20];
   char tok;
}  table[] = {
   "print", PRINT,
   "input", INPUT,
   "if", IF,
   "locate", LOCATE,
   "cls", CLS,
   "end", END,
   "goto", GOTO,
   "then", THEN,
   "else", ELSE,
   "", END
};

/* These are the LABEL TABLE definitions */
struct label {
   char name[LABEL_LENGTH];
   char *p;
};

struct label label_table[NUMBER_OF_LABELS];

/* These are global variables, which make it much easier to work with and insure I get memory freed under any condition */
/* This is a pointer to the actual program in memory */

char *program_name, *program, *prog;
char token[80];
int tok, token_type, var_type;


/************************************************************************/
/**                                                                    **/
/** FUNCTION - main                                                    **/
/**                                                                    **/
/** PURPOSE  - The purpose of the main function is to execute the      **/
/**            Interpretor.                                            **/
/**                                                                    **/
/** INPUT PARMS - argc (Number of arguments)                           **/
/**               argv[1] should be the name of the program            **/
/**                                                                    **/
/** RETURNS     - Returns 0 always                                     **/
/**                                                                    **/
/************************************************************************/
int main(int argc, char *argv[])
{
   int int_var=0;                            /* Initialize integer variable list */
   float float_var=0;                        /* Initialize float variable list   */
   char FromModule[20]="dwbasic";
   char message[40]="This is a test message";
   
   log_message(FromModule, message, CRITICAL, 1, 0);

   error(13);

   printf("Allocating memory.\n");

   /* Allocating memory for program name */
   program_name = malloc(sizeof(char)*(PROGRAM_NAME_SIZE+1)); // need space for '\0'
   if(program_name == NULL) {
      error(11);
   }

   program_name[0]= '\0';

   /* get_memory(program_name);*/

   /* Allocating memory for Program */
   prog = (char*) malloc (MAX_PROGRAM_SIZE);

   if (argc != 2) {
      welcome_screen(program_name, program, prog);
      exit(0);
   }
   else {
      program=prog;
      /*run_program(); */
   }
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  - This function                                           **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
void get_memory(char *program_name)
{

}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  - This function                                           **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
void free_memory()
{
   /* Free memory allocation */
   printf("Freeing memory.\n");
   free(prog);
   free(program_name);
   /* The pointed-to address must not be used again, unless
    re-assigned by another call to malloc. */
   prog = NULL; 
   program_name = NULL; 
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  - This function                                           **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
void get_exp(int *result, float *float_result)
{
   token_type = get_token();
   if (!*token)
   {
      error(3);
      return;
   }
   add_sub(result, float_result);
   putback();
}

void add_sub(int *result, float *float_result)
{
   char operand;
   int hold;
   float float_hold;
   
   mult_div(result, float_result);

   while ((operand = *token) == '+' || operand == '-')
   {
      token_type = get_token();
      mult_div(&hold, &float_hold);
      arith(operand, result, &hold, float_result, &float_hold);
   }
}

void mult_div(int *result, float *float_result)
{
   char operand;
   int hold;
   float float_hold;

   exponent(result, float_result);
   
   while ((operand = *token) == '*' || operand == '/' || operand == '%')
   {
      token_type = get_token();
      exponent(&hold, &float_hold);
      arith(operand, result, &hold, float_result, &float_hold);
   }
}

void exponent(int *result, float *float_result)
{
   int hold;
   float float_hold;
   
   unary(result, float_result);

   if (*token == '^')
   {
      token_type = get_token();
      exponent(&hold, float_result);
      arith('^', result, &hold, float_result, &float_hold);
   }
}

void unary(int *result, float *float_result)
{
   char operand=0;

   if ((token_type == DELIMITER) && (*token == '+' || *token == '-'))
   {
      operand = *token;
      token_type = get_token();
   }
   parans(result, float_result);
   if (operand && operand == '-')
   {
      if (var_type == 1)
      {
         *result = -(*result);
      }
      else 
      {
         *float_result = -(*float_result);
      }
   }
}

void parans(int *result, float *float_result)
{
   if ((*token == '(') && (token_type == DELIMITER))
   {
      token_type = get_token();
      add_sub(result, float_result);
      if (*token != ')')
         error(4);
      token_type = get_token();
   }
   else
      primitive(result, float_result);
}

int raise(int power)
{
   int temp=10;
   
   if (power<=0)
      return 0;
   else
   {
      while (power>1) 
      {
         temp=temp*10;
         power--;
      }
      return temp;
   }
}

void primitive(int *result, float *float_result)
{
   int i=0;
   float temp;
   switch (token_type) {
   case VARIABLE : if (strrchr(token, '#')) {
/***************************************************************************************************

                      *float_result = find_float();
This needs fixed

*****************************************************************************************************/
                      *float_result = 0;  /* This needs replaced */

                      var_type = 2;
                    }
                    else {
/***************************************************************************************************

This needs fixed
                       *result = find_int();

*****************************************************************************************************/
                       *result = 0;   /* This needs replaced */
                       var_type = 1;
                    }
                    token_type = get_token();
                    return;
    case NUMBER :  if (strrchr(token, '.')) {
                      var_type = 2;
                      temp=atof(token);
                      *float_result = temp;
                    } 
                    else {
                       var_type = 1;
                       *result = atoi(token);
                     }
                     token_type = get_token();
                     return;
     default     :   error(3);
   }
}

void arith(char o, int *r, int *h, float *fr, float *fh)
{
   int t, ex;
   float ft, fex;

   if (var_type == 1) {
      switch(o) {
      case '-'     :  *r = *r - *h;
                      break;
      case '+'     :  *r = *r + *h;
                      break;
      case '*'     :  *r = *r * *h;
                      break;
      case '/'     :  *r = (*r) / (*h);
                      break;
      case '%'     :  t = (*r) / (*h);
                      *r = *r - (t * (*h));
                      break;
      case '^'     :  ex = *r;    
                      if (*h == 0)
                      {
                         *r = 1;
                         break;
                      }
                      for (t=*h - 1; t>0; --t)
                          *r = (*r) * ex;
                      break;
       }
   }
   else {
      switch(o) {
      case '-'     :  *fr = *fr - *fh;
                      break;
      case '+'     :  *fr = *fr + *fh;
                      break;
      case '*'     :  *fr = *fr * *fh;
                      break;
      case '/'     :  *fr = (*fr) / (*fh);
                      break;
      case '%'     :  ft = (*fr) / (*fh);
                      *fr = *fr - (ft * (*fh));
                      break;
      case '^'     :  ex = *fr;    
                      if (*fh == 0)
                      {
                         *fr = 1;
                         break;
                      }
                      for (ft=*fh-1; ft>0; --ft)
                          *fr = (*fr) * fex;
                      break;
       }
    }
}
     
      
void putback()
{
   char *t;
   t = token;
   for (;*t;t++)
      program--;
}


void run_program()
{
   int i;
 
   scan_labels();

   tok = EOL;  /* Used to reinitialize tok so as to enter while loop */
   
   while (tok != FINISHED) {
      token_type = get_token();
      if (token_type == VARIABLE)
      {
//         assignment(token);
      }
      else 
      {
         switch (tok) {
            case PRINT   :   do_print();
                             break;
//          case LOCATE  :   do_locate();
//                           break; 
            case CLS     :   do_cls();
                             break;
//          case GOTO    :   do_goto();
//                           break;
//          case IF      :   do_if();
//                           break;
//          case ELSE    :   find_eol();
                             break;
            case END     :   exit(0); 
         }
      }  
   }
}


void do_print()
{
   printf("Executing PRINT command\n");
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - get_next_label                                          **/
/**                                                                    **/
/** PURPOSE  - This function                                           **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
int get_next_label(char *s)
{
   int t;
   for (t=0; t< NUMBER_OF_LABELS; ++t) {
      if (label_table[t].name[0] == 0)
         return t;
      if (!strcmp(label_table[t].name,s))
         return -2;  /* Duplicate Label Found */
   }
   return -1;  /* Label table is full */
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - is_digit                                                **/
/**                                                                    **/
/** PURPOSE  - To distinguish if the next token/character is a digit   **/
/**                                                                    **/
/** INPUT PARMS - char c - the character to check                      **/
/**                                                                    **/
/** RETURNS     - Returns 1 if is whitespace, 0 if not                 **/
/**                                                                    **/
/************************************************************************/
int is_digit(char c)
{
   if (c=='0' || c=='1' || c=='2' || c=='3' || c=='4' || c=='5' || c=='6' || c=='7' || c=='8' || c=='9')
      return 1;
   else 
      return 0;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - is_alpha                                                **/
/**                                                                    **/
/** PURPOSE  - To distinguish if the next token/character is ALPHA     **/
/**                                                                    **/
/** INPUT PARMS - char c - the character to check                      **/
/**                                                                    **/
/** RETURNS     - Returns 1 if is whitespace, 0 if not                 **/
/**                                                                    **/
/************************************************************************/
int is_alpha(char c)
{
   if (c>='a' && c<='z')
      return 1;
   else 
      return 0;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - is_whitespace                                           **/
/**                                                                    **/
/** PURPOSE  - To distinguish if the next token/character is whitespc. **/
/**                                                                    **/
/** INPUT PARMS - char c - the character to check                      **/
/**                                                                    **/
/** RETURNS     - Returns 1 if is whitespace, 0 if not                 **/
/**                                                                    **/
/************************************************************************/
int is_whitespace(char c)
{
   if (c==' ' || c=='\t')
      return 1;
   else 
      return 0;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION - is_delim                                                **/
/**                                                                    **/
/** PURPOSE  - To distinguish if the next token/character is a delim.  **/
/**                                                                    **/
/** INPUT PARMS - char c - the character to check                      **/
/**                                                                    **/
/** RETURNS     - Returns 1 if is a delim, 0 if not                    **/
/**                                                                    **/
/************************************************************************/
int is_delim(char c)
{
   if (strchr(" ;,+-<>/*%^=()", c) || c == 9 || c=='\r' || c==0)
      return 1;
   else 
      return 0;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
char *find_label(char *s)
{
   int t;
   for (t=0; t< NUMBER_OF_LABELS; ++t) {
      if (!strcmp(label_table[t].name, s))
         return label_table[t].p;
   }
   return '\0'; /* ERROR occurred */
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
void label_init()
{
   int t;

   printf("DEBUG: Initializing label table\n");

   for (t=0; t<NUMBER_OF_LABELS; ++t)
      label_table[t].name[0] = '\0';
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
void find_eol()
{
   while (*program != '\n' && *program != '\0')
      ++program;
   if (*program)
      program++;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
int error(int err)
{
   int i;
   free_memory();
   printf("HALTING EXECUTION, ERROR %d", err);
   switch (err)
   {   case 1 : { printf(", File not found.\n");
                  break;
                }
       case 2 : { printf(", = expected in assignment statement.\n");
                  break;
                }
       case 3 : { printf(", syntax error.\n");
                  break;
                }
       case 4 : { printf(", unbalanced parantheses.\n");
                  break;
                }
       case 5 : { printf(", equals sign expected in assignment statement.\n");
                  break;
                }
       case 6 : { printf(", comma expected in locate statement.\n");
                  break;
                }
       case 7 : { printf(", label table full.\n");
                  break;
                }
       case 8 : { printf(", duplicate label(s).\n");
                  break;
                }
       case 9 : { printf(", undefined label.\n");
                  break;
                }
       case 10 :{ printf(", couldn't allocate memory for program.\n");
                  break;
                }
       case 11 :{ printf(", couldn't allocate memory for program name.\n");
                  break;
                }
       case 12 :{ printf(", program size to large for MAX_PROGRAM_SIZE.\n");
                  break;
                }
       case 13 :{ printf(", Failure to compute the current time in log function.\n");
                  break;
                }
       case 14 :{ printf(", Failure to convert the current time in log function.\n");
                  break;
                }
       case 15 :{ printf(", localtime error in log function.\n");
                  break;
                }
       case 16 :{ printf(", strftime returned 0 in log function.\n");
                  break;
                }
   }
   printf("Press any key to continue.\n");
   i=getc(stdin);
   halt_execution(err);
   return err;
}

void halt_execution(int errno)
{
   exit(errno);
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
int look_up(char *s)
{
   int i;
   char *p;
   /* Convert to lowercase */
   p = s;
   while (*p)
   {
      *p = tolower(*p);
      p++;
   }

   /* Search Table */

   for (i=0; *table[i].command;i++)
      if (!strcmp(table[i].command, s))
         return (table[i].tok);

   return 0;
}

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
int get_token()
{
   char *temp;
   temp=token;

   if (*program == '\0')                                            /* End of Program */
   {
      *token = 0;
      tok=FINISHED;
      token_type=DELIMITER;
   }

   while (is_whitespace(*program)) ++program;

   if (*program == '\r')
   {
      program = program + 2;
      tok = EOL;
      *token = '\r';
      token[1] = '\n';
      token[2] = 0;
      token_type=DELIMITER;
   }

   if (strchr("+-*^/%=;(),><", *program))                           /* It's a Delimiter */
   {
      *temp = *program;
      program++;
      temp++;
      *temp=0;
      token_type=DELIMITER;
   }

   if (*program =='"')                                              /* It's a Quote */
   {
      program++;
      while (*program!='"' && *program!='\r')
         *temp++=*program++;
      *temp='\0';
      token_type=QUOTE;
   }

   if (is_digit(*program))                                          /*  It's a number */
   {
      while (!is_delim(*program))
         *temp++=*program++;
      *temp='\0';
      token_type=NUMBER;
   }

   if (is_alpha(*program))                                          /*  It's a variable */
   {
      while (!is_delim(*program))
         *temp++=*program++;
      token_type = STRING;
   }

   *temp = '\0';

   if (token_type == STRING)
   {
      tok = look_up(token);
      if (!tok)
         token_type = VARIABLE;
      else
         token_type = COMMAND;
   }

   return (token_type);
}


/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
int scan_labels()
{
   int address;
   char *temp;
   label_init();                                                            /* Call label_init */
   
   temp = program;

   /*token_type = get_token();*/                                                /* Call get_token */

   if (token_type ==NUMBER) {
      strcpy(label_table[0].name, token);
      label_table[0].p=program;
   }

   find_eol();                                                              /* Call find_eol */

   do {
      token_type = get_token();
      if (token_type == NUMBER) {
         address = get_next_label(token);
         if (address == -1 || address == -2) {
            if (address == -1)
               error(7);
            if (address == -2)
               error(8);
         }
         strcpy(label_table[address].name, token);
         label_table[address].p = program;
      }
      if (tok != EOL)
         find_eol();                                                        /* Call find_eol */
   } while (tok != FINISHED);
   program = temp;


   /* ADDED THIS RETURN FOR WARNING - NEED TO CORRECT */
   return 0;
}

/* This is a generic header to use for comments when adding a new function */

/************************************************************************/
/**                                                                    **/
/** FUNCTION -                                                         **/
/**                                                                    **/
/** PURPOSE  -                                                         **/
/**                                                                    **/
/**                                                                    **/
/** INPUT PARMS -                                                      **/
/**                                                                    **/
/** RETURNS     - Returns                                              **/
/**                                                                    **/
/************************************************************************/
