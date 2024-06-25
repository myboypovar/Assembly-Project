#include "output.h"

/* scans the file and prints the output files.
   input: argv = file name
   sym_head, data_head, code_head = pointers to the linked lists.
   ic = instructions counter , dc = data counter. */
void secondPass(char *argv, SymbolTable *sym_head, DataList *data_head, CodeList *code_head, int *ic, int *dc);

/* immediate addressing case, prints the input string.
   input: ptr = the input string.
   ic = the address.
   fp = file output. */
void gotImmediate(char *ptr, int *ic, FILE *fp);

/* direct or record-access addressing case, checks which addressing it is and if the labels are correct.
   input: ptr = the argument string.
   tmp_ic = pointer to the address.
   fp_ob = the object file, fp_ext = the extern file.
   lineNum,argv = for error if the label was not found.
   sym_head = symbol table pointer.
   add = addressing.
   returns 0 if error, returns 1 if successful, returns 2 if got an external symbol. */
int gotLabel(char *ptr, int *tmp_ic, FILE *fp_ob, FILE *fp_ext, int lineNum, char *argv, SymbolTable *sym_head, int add);

/* register addressing case.
   input: dest = destination addressing, src = source addressing.
   tmp_ic = pointer to the address.
   fp = file pointer to the output file. */
void gotRegister(int dest, int src, int *tmp_ic, FILE *fp);

