#include "macro.h"
#include "first_pass.h"
#include "second_pass.h"

int main(int argc, char *argv[])
{
	int i;
	for (i = 1; i < argc; i++) {
		int error = 0, ic = MEMORY_START, dc = 0; /* ic = instructions counter, dc = data counter. */
		error = readFile(argv[i]); /* check for macro and convert the file to ".am" */
		if (!error) {
			SymbolTable *sym_head = (SymbolTable *)mallocCheck(sizeof(SymbolTable));
			DataList *data_head = (DataList *)mallocCheck(sizeof(DataList));
			CodeList *code_head = (CodeList *)mallocCheck(sizeof(CodeList));
			sym_head -> next = sym_head;
			data_head -> next = data_head;
			code_head -> next = code_head;  	/*fill the symbol table and check for erros */
			error = firstPass(argv[i], sym_head, data_head, code_head, &ic, &dc); 
			if (!error) {				/* make the output */
				secondPass(argv[i], sym_head, data_head, code_head, &ic, &dc);
			}
		}
	} 
	return 0;
}
