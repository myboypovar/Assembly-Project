#include "utility.h"

#define MAX_MACRO_LINES 100

/* linked list for implementing the macro */
typedef struct Item {
	char macro_name[LINE_LENGTH];
	char *content[MAX_MACRO_LINES];
	struct Item *next;
	} Macro;

/* starts reading the file while scanning for macro, the function converts the ".as" file to ".am" and implements the macros,
   if successful return 0, if the macro name is invalid or the file doesn't exist returns 1. */
int readFile(char *argv);

/* processing the file and checks if a macro exists and if there is a need to apply a macro. returns 1 if
   the macro is invalid, else returns 0. */
int checkForMacro(FILE *fpr, Macro *head, FILE *fpw, char *argv);

/* check if the string pointer contains a macro's name, if it is, the function replaces the macro's name from the ".as" file
   with the macro's content to the ".am" file. if a macro's name was found returns 1, else 0. */
int writeMacro(char *ptr, FILE *fpw, Macro *head);

/* free the Macro's linked list */
void freeMacroList(Macro *head);
