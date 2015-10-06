
// This is a module of log related functions that can be used for various logging requirements.
//
//

#include "dwbasic.h"

/************************************************************************/
/**                                                                    **/
/** FUNCTION - log                                                     **/
/**                                                                    **/
/** PURPOSE  - To log a message to a log file with various details.    **/
/**                                                                    **/
/** INPUT PARMS - FromModule - This is the name of the module that is  **/
/**                            requesting the logging.                 **/
/**                                                                    **/
/**               message - This is the text that gets logged.         **/
/**                                                                    **/
/**               severity     Informational - 1                       **/
/**                            Warning       - 2                       **/
/**                            Critical      - 3                       **/
/**                                                                    **/
/**               console - Boolean value 1 to log to console, 0 to    **/
/**                         not.                                       **/
/**               logfile - Boolean value 1 to log to file, 0 to not.  **/
/**                                                                    **/
/** RETURNS     - Returns nothing.                                     **/
/**                                                                    **/
/************************************************************************/

void log_message(char *FromModule,char *message, int severity, int console, int logfile)
{
   time_t current_time;
   char time_string[200];
   struct tm *tmp;
   char fname[80]="log-";
   char sevlevel[20]="\0";

   strcat(fname, FromModule);
   strcat(fname, ".txt");

   /* Obtain current time as seconds elapsed since the Epoch. */
   current_time = time(NULL);

   tmp = localtime(&current_time);
   if (tmp == NULL) {
        error(15);
   }

   if (strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", tmp) == 0) {
        error(16);
    }

   printf("Result string is \"%s\"\n", time_string);
 
   if (time_string == NULL)
   {
      error(14);
   }
 
   switch (severity) {
      case INFORMATIONAL : strcpy(sevlevel, "INFORMATIONAL");
                           break;
      case WARNING       : strcpy(sevlevel, "WARNING");
                           break;
      case CRITICAL      : strcpy(sevlevel, "CRITICAL");
                           break;
      default            : strcat(sevlevel, "INVALID-SEVERITY");
                           break;
   }


   printf("%s:%s:%ld:Log from %s: %s\n", sevlevel, time_string, current_time, FromModule, message);
   printf("Logfile name is %s.\n", fname);

   //if (!(fp=fopen(filename, "rb")))
      //return 0;
}
