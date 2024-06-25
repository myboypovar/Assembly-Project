#include "symbol_table.h"
#include "utility.h"

/* adds a new symbol to the symbol table */
int addSymbol(char symName[], int type, int address, SymbolTable *head)
{
	SymbolTable *sym_ptr = head;
	SymbolTable *newSym;

	if (head -> next == head) { /* if the head is empty */
		strcpy(head->symbolName, symName);
		head -> type = type;
		head -> address = address;
		head -> next = NULL;
	}
	else {
		while (1) {
			if (!strcmp(symName, sym_ptr -> symbolName)) { /* if the label already exists */
				if (sym_ptr -> type == EXTERN && type != EXTERN) {
					return -1; /* the label was assigned before as extern */
				}
				return 0;
			}
			if (sym_ptr -> next == NULL) {
				break;
			}
			sym_ptr = sym_ptr -> next;
		}
		newSym = (SymbolTable *)mallocCheck(sizeof(SymbolTable));
		sym_ptr -> next = newSym;
		strcpy(newSym->symbolName, symName);
		newSym -> type = type;
		newSym -> address = address;
		newSym -> next = NULL;
	}
	return 1;
}

void printSymbolTable(SymbolTable *head)
{
	SymbolTable *ptr = head;
	if (head -> next == head) {
		return;
	}
	while (ptr) {
		printf("\nLabel:%s\nAddress:%d\nType:%d\n",ptr->symbolName,ptr->address,
			ptr->type);
		ptr = ptr->next;
	}
}
/* adds the instruction counter to all the data symbols, to separate the data from the instructions. */
void updateSymbolTable(SymbolTable *head, int ic)
{
	SymbolTable *ptr = head;
	if (head -> next == head) {
		return;
	}
	while (ptr) {
		if (ptr -> type == DATA || ptr -> type == STRING || ptr -> type == STRUCT) { /* update only data symbols */
			ptr -> address += ic;
		}
		ptr = ptr -> next;
	}
}

/* searches for the label and returns its address */
int searchLabel(char label[], int lineNum, char *argv, SymbolTable *head, int *ARE)
{
	SymbolTable *ptr = head;
	int add = 0; /* address */
	if (head -> next == head) {
		printf("ERROR: in file \"%s\" in line %d.\nthe label \"%s\" doesn't exist.\n",argv,lineNum,label);
		return -1;
	}
	while (ptr) {
		if (!strcmp(label,ptr->symbolName)) {
			if (ptr -> type != EXTERN) {
				add = ptr -> address;
			} else {   		/* external label */
				*ARE = 1;       /* "01" in binary */
				return 0;
			}
			
		}
		ptr = ptr -> next;
	}
	if (!add) { /* label not found */
		printf("ERROR: in file \"%s\" in line %d.\nthe label \"%s\" doesn't exist.\n",argv,lineNum,label);
		return -1;
	}
	return add;
}

/* frees the symbol table */
void freeSymbolTable(SymbolTable *head)
{
	SymbolTable *ptr = head;
	if (head -> next == head) {
		free(head);
		return;
	}
	while (ptr) {
		SymbolTable *tmp = ptr -> next;
		free(ptr);
		ptr = tmp;
	}
}












