#include "symbol_table.h"
#include "decode.h"
#include "instructions.h"
#include "utility.h"
#include "second_pass.h"

/* scans the ".am" file and prints the output files */
void secondPass(char *argv, SymbolTable *sym_head, DataList *data_head, CodeList *code_head, int *ic, int *dc)
{
	FILE *fp, *fp_ob, *fp_ext, *fp_ent; /* fp for input (.am), fp_ob for object output, fp_ext for extern, fp_ent for entry*/
	CodeList *code_ptr = code_head;
	char *file_name, line[LINE_LENGTH], *fileOb, *fileEnt, *fileExt; /* file names and the line */
	/* tmp_ic for the address, gotEntry = if got an entry, gotExtern = if got an extern. */
	int lineNum = 0, tmp_ic = MEMORY_START, error = 0, gotEntry = 0 , gotExtern = 0;
	
	file_name = strmalloccat(argv, ".am");
	fileOb = strmalloccat(argv, ".ob");
	fileEnt = strmalloccat(argv, ".ent");
	fileExt = strmalloccat(argv, ".ext");
	fp = fopen(file_name,"r"); /* no error previously = file exists */
	fp_ob = fopen(fileOb,"w");
	fp_ent = fopen(fileEnt,"w");
	fp_ext = fopen(fileExt,"w");
	
	printHead(*ic,*dc,fp_ob); /* prints the number of the instructions and the data */
	while (fgets(line,LINE_LENGTH,fp)) {
		char *ptr; /* pointer to the parsed line */
		int type = 0, arguments = 0;
		
		lineNum++;
		ptr = strtok(line," \t,\n");
		if (!ptr || *ptr == ';') {
			continue;
		}
		if (ptr[strlen(ptr)-1] == ':') { /* skip label */
			ptr = strtok(NULL," \t,\n");
		}
		type = checkForType(ptr,lineNum,argv);
		if (type != ENTRY && type) { /* if it's not an instruction or .entry. */
			continue;
		}
		ptr = strtok(NULL," \t,\n"); /* skip word */
		if (type == ENTRY) {
			if (!printEntry(ptr,sym_head,fp_ent,lineNum,argv)) { /*print the entry */
				error = 1; /* symbol was already assigned as extern. */
			}
			gotEntry = 1;
			continue;
		}
		arguments = numOfArguments(code_ptr -> op_code + 1); /* index is different than the opcode enum */
		
		printAddress(tmp_ic,fp_ob);
		printInstruction(code_ptr,fp_ob);
		tmp_ic++;
		
		if (arguments == 1) { /* only destination argument */
			int addressing = code_ptr -> dest_addressing;
			if (addressing == IMMEDIATE) {
				gotImmediate(ptr,&tmp_ic,fp_ob);
			}
			if (addressing == DIRECT || addressing == RECORD_ACCESS) {
				int check;
				check = gotLabel(ptr,&tmp_ic,fp_ob,fp_ext,lineNum,argv,sym_head,addressing);
				if (!check) {
					error = 1;
				}
				if (check == 2) { /* gotLabel return 2 == printed external */
					gotExtern = 1;
				}
			}
			if (addressing == REGISTER) {
				int dest;
				dest = isRegister(ptr,0) - 1; /* register enum is different from the index */
				gotRegister(dest,0,&tmp_ic,fp_ob);
			}
		} /* end of arguments = 1 */
		if (arguments == 2) {
			int srcAdd, destAdd;
			srcAdd = code_ptr -> src_addressing;
			destAdd = code_ptr -> dest_addressing;
			if (srcAdd == REGISTER && destAdd == REGISTER) { /* both arguments are registers */
				int src, dest;
				src = isRegister(ptr,0) - 1;
				ptr = strtok(NULL," \t,\n"); /* checked if the second argument exists in first_pass */
				dest = isRegister(ptr,0) - 1;
				gotRegister(dest,src,&tmp_ic,fp_ob);
				code_ptr = code_ptr -> next;
				continue;
			}
			if (srcAdd == IMMEDIATE) {
				gotImmediate(ptr,&tmp_ic,fp_ob);
			}
			if (srcAdd == DIRECT || srcAdd == RECORD_ACCESS) {
				int check;
				check = gotLabel(ptr,&tmp_ic,fp_ob,fp_ext,lineNum,argv,sym_head,srcAdd);
				if (!check) {
					error = 1;
				}
				if (check == 2) { 
					gotExtern = 1;
				}
			}
			if (srcAdd == REGISTER) {
				int src;
				src = isRegister(ptr,0) - 1;
				gotRegister(0,src,&tmp_ic,fp_ob);
			}
			ptr = strtok(NULL," \t,\n");	/* move to the next argument */
			if (destAdd == IMMEDIATE) {
				gotImmediate(ptr,&tmp_ic,fp_ob);
			}
			if (destAdd == DIRECT || destAdd == RECORD_ACCESS) {
				int check;
				check = gotLabel(ptr,&tmp_ic,fp_ob,fp_ext,lineNum,argv,sym_head,destAdd);
				if (!check) {
					error = 1;
				}
				if (check == 2) { 
					gotExtern = 1;
				}
			}
			if (destAdd == REGISTER) {
				int dest;
				dest = isRegister(ptr,0) - 1;
				gotRegister(dest,0,&tmp_ic,fp_ob);
			}
		} /* end of arguments = 2 */
	code_ptr = code_ptr -> next;  /* go to the next instruction */
	} /*end of while */
	if (error) {
		remove(file_name);
		remove(fileOb);
		remove(fileExt);
		remove(fileEnt);
		exit(0);
	}
	printDataList(tmp_ic, data_head, fp_ob);
	if (!gotEntry) {
		remove(fileEnt);
	}
	if (!gotExtern) {
		remove(fileExt);
	}
	freeDecode(code_head,data_head);
	freeSymbolTable(sym_head);
	fclose(fp);
	fclose(fp_ob);
	fclose(fp_ent);
	fclose(fp_ext);
	free(file_name);
	free(fileOb);
	free(fileEnt);
	free(fileExt);
}

/* immediate case */
void gotImmediate(char *ptr, int *ic, FILE *fp)
{
	int gotMinus = 0, num;
	Word input;
	ptr++;
	if (*ptr == '+' || *ptr == '-') {
		if (*ptr == '-') {
			gotMinus = 1;
		}
		ptr++;
	}
	if (gotMinus) {
		num = -atoi(ptr);
	} else {
		num = atoi(ptr);
	}
	input.ARE = 0;
	input.value = num;
	printWord(input,*ic,fp);
	*ic += 1;
}

/* direct or record-access case */
int gotLabel(char *ptr, int *tmp_ic ,FILE *fp_ob, FILE *fp_ext, int lineNum, char *argv, SymbolTable *sym_head, int add)
{
	char label[SYMBOL_LENGTH];
	int i, address, are = 2;/* default is "10" for not external*/
	Word input;

	memset(label,'\0',SYMBOL_LENGTH);
	if (add == DIRECT) {
		strcpy(label,ptr);
	} else { /* struct */
		for (i = 0; ptr[i]; i++) {
			if (ptr[i] == '.') {
				strncpy(label,ptr,i); /* copy the label's name util the dot */
				break;
			}
		}
	}
	if ((address = searchLabel(label,lineNum,argv,sym_head,&are)) == - 1) { /* label not found */
		return 0;
	}
	if (are == 1) { /* got an external label */
		char ext[3];
		ext[0] = convertToBase32(*tmp_ic / 32); /* print the external label */
		ext[1] = convertToBase32(*tmp_ic % 32);
		ext[2] = '\0';
		fprintf(fp_ext,"%s\t%s\n",label,ext);
	}
	input.ARE = are;
	input.value = address;
	printWord(input,*tmp_ic,fp_ob);
	*tmp_ic += 1;
	if (add == RECORD_ACCESS) { /* print the field number */
		Word field;
		int number;
		ptr += i+1;
		number = atoi(ptr); /* get 1 or 2 */
		field.ARE = 0;
		field.value = number;
		printWord(field,*tmp_ic,fp_ob);
		*tmp_ic += 1;
	}
	if (are == 1) {
		return 2; /* flag for got external */
	}
	return 1;
}

/* register addressing case */
void gotRegister(int dest, int src, int *tmp_ic, FILE *fp)
{
	Register reg;
	reg.ARE = 0;
	reg.dest = dest;
	reg.src = src;
	printRegister(reg,*tmp_ic,fp);
	*tmp_ic += 1;
}










