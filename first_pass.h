#include "symbol_table.h"
#include "decode.h"

/* adds symbols, instructions and data to the memory.
   input: argv = file name, all the linked lists heads: symbol table, data list, code list. ic = instruction counter,
   dc = data counter.
   returns 0 if no errors, 1 if error found */ 
int firstPass(char *argv, SymbolTable *sym_head, DataList *data_head, CodeList *code_head, int *ic, int *dc);

/* checks if a label exists, checks if the label's syntax is valid and if the label is not a reserved word.
   input: ptr = pointer to the parsed line, labelName = array to store the label, type = if struct ,the function acts differently.
   returns 1 if a label was found, 0 if no label, -1 if the label's name is a reserved word or invalid. */
int checkForLabel(char *ptr, char labelName[], int lineNum, char *argv, int type);

/* checks if the data is valid and stores it in the data's linked list.
   input:
   ptr = pointer to the parsed line, linePtr = pointer to the line, used for filterInt to store the data.
   lineNum = line number for error, argv = file name for error.
   type = if its struct the function spots a colon without an error.
   data_head = to store the data.
   returns 0 if error, -1 if its struct + missing a colon, returns the number of words from the guidance. */
int getDataLength(char *ptr, char *linePtr, int lineNum, char *argv, int type, DataList *data_head);

/* checks if the string is valid.
   input: ptr = pointer to the parsed line, lineNum,argv for error, 
   data_head = to store the data.
   returns the number of characters of the string, returns 0 if error. */
int getStringLength(char *ptr, int lineNum, char *argv, DataList *data_head);

/* uses getDataLength() and getStringLength() to get the combined number of words, and add the words to the data list.
   input: ptr = pointer to the parsed line, linePtr = pointer to the line, lineNum and argv for error.
   type = for getDataLength().
   data_head = for the data list.
   return the number of words from the data and the string, retuns 0 if got an error. */
int getStructLength(char *ptr, char *linePtr, int lineNum, char *argv, int type, DataList *data_head);


/* filters the integer from the line, and stores it in the data's list.
   input: ptr = pointer the the line (not parsed)
          data_head = the data list pointer. */
void filterInt(char *ptr, DataList *data_head);

