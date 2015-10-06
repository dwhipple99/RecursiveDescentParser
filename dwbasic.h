
/* These are some constants based on the size programs the system will execute */

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

/* These are the required INCLUDE files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* These are the TOKEN type definitions */
#define DELIMITER 1
#define VARIABLE 2
#define NUMBER 3
#define COMMAND 4
#define STRING 5
#define QUOTE 6

#define EOL 9
#define FINISHED 10

/* These are the defined commands in the language */
#define PRINT 1
#define INPUT 2
#define IF 3
#define LOCATE 4
#define CLS 5
#define GOTO 6
#define THEN 7
#define ELSE 8
#define END 13

/* These are the various log levels */
#define INFORMATIONAL 1
#define WARNING 2
#define CRITICAL 3

#define MAX_PROGRAM_SIZE 65536
#define PROGRAM_NAME_SIZE 255
#define LABEL_LENGTH 255
#define NUMBER_OF_LABELS 5000
#define VERSION 0.1

int error(int err);
void free_memory();
int get_token();
void add_sub(int *result, float *float_result);
void get_exp(int *result, float *float_result);
void mult_div(int *result, float *float_result);
void exponent(int *result, float *float_result);
void parans(int *result, float *float_result);
int raise(int power);
void primitive(int *result, float *float_result);;
void arith(char o, int *r, int *h, float *fr, float *fh);
void putback();
void unary(int *result, float *float_result);
int scan_labels();
void do_cls();
void do_print();
void list(char *program);
int error(int err);
void get_memory(char *program_name);
int load(char *filename, char *prog);

int welcome_screen();
void halt_execution(int errno);
void log_message(char *FromModule,char *message, int severity, int console, int logfile);


