
#define SYMBOL_LENGTH 30 /*assumption from the project instructions */

typedef struct Symb {
	char symbolName[SYMBOL_LENGTH];
	int address; /* ic for instructions, id for data */
	int type; /* instruction, entry extern, data string struct */
	struct Symb *next;
	} SymbolTable;

/* adds a symbol to the symbol table.
   input: symName = the symbol's name, type = code or data or ext or ent.
   address = the address of the symbol.
   head = pointer to the symbol table list.
   returns 1 if successful, retruns 0 if label already exists, -1 if label already exsits as extern. */
int addSymbol(char symName[], int type, int address, SymbolTable *head);

/* prints the symbol table to stdout */
void printSymbolTable(SymbolTable *head);

/* separates the data symbols from the instruction symbols by adding the last instruction's address to all the
   data symbols.
   input: head = pointer the symbol table, ic = the last instruction address.*/
void updateSymbolTable(SymbolTable *head, int ic);

/* searches for the input label, if the label is external, the function modifies the input ARE.
   input: label = label's name,  lineNum,argv = for error
   head = symbol table pointer
   ARE = changes the value to 1 if external.
   returns the address of the label, if the label is external returns 0, if error returns -1. */
int searchLabel(char label[], int lineNum, char *argv, SymbolTable *head, int *ARE);

/* frees the symbol table linked list */
void freeSymbolTable(SymbolTable *head);

