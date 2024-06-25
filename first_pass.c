#include "first_pass.h"
#include "instructions.h"
#include "utility.h"

/* checks for errors throughout the file, adds symbols to the symbol table, adds data to the data's list and code to
   the code's list. if no errors found returns 0 else 1. */
int firstPass(char *argv, SymbolTable *sym_head, DataList *data_head, CodeList *code_head, int *ic, int *dc)
{
	FILE *fp;
	int lineNum = 0, error = 0;
	char line[LINE_LENGTH];
	char *file_name;
	
	file_name = strmalloccat(argv, ".am");
	if (!(fp = fopen(file_name,"r"))) {
		printf("ERROR: the file \"%s\" does not exist.\n",argv);
		return 1;
	}
	while (fgets(line,LINE_LENGTH,fp)) {
		int label, opCode, type = 0, numOfWords = 0; /* numOfWords = how many lines of code to add */
		char tmpLine[LINE_LENGTH]; /* copied line for parsing (strtok) */
		char labelName[SYMBOL_LENGTH] = {'\0'};
		char *ptr, *linePtr = line; /* ptr = pointer the the parsed line, linePtr = pointer to the actual line */

		lineNum++;
		strcpy(tmpLine,line);
		if (!(ptr = strtok(tmpLine," \t\n")) || *ptr == ';') { /* skip empty line or comment */
			continue;
		}
		label = checkForLabel(ptr,labelName,lineNum,argv,type); /* if a label exists */
		if (label == -1) {
			error = 1;
		}
		if (label) { /* move the pointer to after the label (if the label exists) */
			if (!(ptr = strtok(NULL," \t\n"))) {
				if (label > 0) {
					printf("ERROR: label is empty in file \"%s\" in line %d:\n",argv,lineNum);
				}
				error = 1;
				continue;
			}
		}
		if ((type = checkForType(ptr,lineNum,argv))) { /* got a guidance's name */
			if (!(ptr = strtok(NULL," \t\n"))) {
				printf("ERROR: missing argument in file \"%s\" in line %d:\n",argv,lineNum);
				error = 1;
				continue;
			}
			if (type == DATA) {
				linePtr = skipToData(linePtr,label); /* skip to after ".data" */
				numOfWords = getDataLength(ptr,linePtr,lineNum,argv,type,data_head);
			}
			if (type == STRING) {
				linePtr = skipToData(linePtr,label);
				numOfWords = getStringLength(linePtr,lineNum,argv,data_head);
			}
			if (type == STRUCT) {
				linePtr = skipToData(linePtr,label);
				numOfWords = getStructLength(ptr,linePtr,lineNum,argv,type,data_head);
			}
			if (type == DATA || type == STRING || type == STRUCT) {
				if (!numOfWords) { /* had an error */
					error = 1;
				}
				else if (label == 1) {  /* add to the symbol table */
					int sym; /* sym values: -1,0,1 */
					sym = addSymbol(labelName, type, *dc, sym_head);
					if (!sym) {
						printf("ERROR: in file \"%s\" in line %d:\n"
							"the label \"%s\" already exists.\n",argv,lineNum,labelName);
						error = 1;
					}
					if (sym == -1) {
						printf("ERROR: in file \"%s\" in line %d:\n"
							"the label \"%s\" was already assigned"
							" as \".extern\".\n",argv,lineNum,labelName);
						error = 1;
					}
				}
				*dc += numOfWords;
			}
			if (type == ENTRY) {
				if (checkForLabel(ptr,labelName,lineNum,argv,type) < 1) { /*check the argument name */
					error = 1;
				}
				continue;
			}
			if (type == EXTERN) {
				label = checkForLabel(ptr,labelName,lineNum,argv,type);
				if ((ptr = strtok(NULL," \t\n"))) {
					printf("ERROR: extraneous text after command."
						" in file \"%s\" in line %d.\n",argv,lineNum);
					error = 1;
				}
				else if (label != 1) { /* invalid label name */
					error = 1;
				} else {
					if (!addSymbol(labelName, type, 0, sym_head)) {
						printf("ERROR: in file \"%s\" in line %d:\n"
							"the label \"%s\" already exists, therefore cannot be assigned"
							" as an external label.\n",argv,lineNum,labelName);
						error = 1;
					}
				}
			}
		} /* end of type */
		else if ((opCode = isOpCode(ptr)) > 0) { /* if it's an instruction */
			numOfWords = handleInstruction(ptr, opCode, lineNum, argv, code_head);
			if (!numOfWords) {
				error = 1;
			}
			if (label == 1) { /* add to the symbol's table */
				int sym;
				sym = addSymbol(labelName, CODE, *ic, sym_head);
				if (!sym) {
					printf("ERROR: in file \"%s\" in line %d.\n"
						"the label \"%s\" already exists.\n",argv,lineNum,labelName);
					error = 1;
				} 
				if (sym == -1) {
					printf("ERROR: in file \"%s\" in line %d.\n"
						"the label \"%s\" was already assigned as \".extern\".\n",argv,lineNum,labelName);
					error = 1;
				}
			}
			*ic += numOfWords;
		} /* end of opcode */
		else {
			printf("ERROR: undefined command name in file \"%s\" in line %d.\n"
				"the command \"%s\" doesn't exist.\n",argv,lineNum,ptr);
			error = 1;
		}
	} /* end of while */
	if (error) { /* free allocated space */
		freeSymbolTable(sym_head);
		freeDecode(code_head,data_head);
		fclose(fp);
		remove(file_name);
		free(file_name);
		return error;
	}
	updateSymbolTable(sym_head, *ic);
	fclose(fp);
	free(file_name);
	return error;
}

/* checks if the string is a label */
int checkForLabel(char *ptr, char labelName[], int lineNum, char *argv, int type)
{
	int i, n = strlen(ptr), label = 0, opcode = 0, reg = 0; /* n = length of the string, reg = register */
	int k = n; /* k is the length of the string without the colon */
	if (ptr[n-1] == ':' || type == ENTRY || type == EXTERN) {
		if (ptr[n-1] == ':' && type != ENTRY && type != EXTERN) {
			k = n-1;
		}
		if ((opcode = isOpCode(ptr))) {
			if (opcode == -1 && (type == ENTRY || type == EXTERN)) { /* with colon for entry or extern */
				printf("ERROR: invalid label name in file \"%s\" in line %d\n"
					"the label must contain only alphanumeric characters.\n" ,argv,lineNum);
					return -1;
			}
			printf("ERROR: invalid label name in file \"%s\" in line %d\n"
				"opcodes cannot serve as a label's name\n",argv,lineNum);
			label = -1;
		}

		else if ((reg = isRegister(ptr,0))) {
			if (reg == -1 && (type == ENTRY || type == EXTERN)) { /* with colon for entry of e*/
				printf("ERROR: invalid label name in file \"%s\" in line %d\n"
					"the label must contain only alphanumeric characters.\n" ,argv,lineNum);
					return -1;
			}
			printf("ERROR: invalid label name in file \"%s\" in line %d\n"
				"register names cannot serve as a label's name\n" ,argv,lineNum);
			label = -1;	
		}
		
		else if (isalpha(*ptr)) { /* label must start with an alphabetic letter */
			for (i=1; i < k; i++) {
				if (!isalnum(ptr[i])) { /* all the letters should be alphanumenric */
					printf("ERROR: invalid label name in file \"%s\" in line %d\n"
					"the label must contain only alphanumeric characters.\n" ,argv,lineNum);
					return -1;
				}
			}
			strncpy(labelName,ptr,k); /* copy the label to the memory */
			return 1;
		}
		else {
			printf("ERROR: invalid label name in file \"%s\" in line %d\n"
				"the label must start with an alphabetic character.\n" ,argv,lineNum);
			label = -1;
		}
	}
	return label;
}

/* checks if the input is valid and stores the data in a data's linked list, returns the number of the words */
int getDataLength(char *ptr, char *linePtr, int lineNum, char *argv, int type, DataList *data_head)
{
	int i, count = 0, error = 0, gotSign = 0, gotNum = 0;
	if (*ptr == ',') {
		printf("ERROR: illegal comma after .data in file \"%s\" in line %d\n",argv,lineNum);
		error = 1;
		ptr++;
	}
	while (ptr) {
		if (*ptr != ',' && gotNum) { /*if the argument is ...,1 2,... */
			printf("ERROR: missing comma in file \"%s\" in line %d\n",argv,lineNum);
			return 0;
		}
		for (i=0; i < strlen(ptr); i++) {
			if (ptr[i] == ',') {
				if (!gotNum) {				
					printf("ERROR: multiple consecutive commas in file \"%s\" "
					"in line %d\n",argv,lineNum);
					error = 1;
				}
				else { /* go to the next number */
					gotSign = 0;
					gotNum = 0;
					count++;
				}
			}
			else if (ptr[i] == '+' || ptr[i] == '-') {
				if (gotNum) { 
					printf("ERROR: missing comma in file \"%s\" in line %d\n",argv,lineNum);
					error = 1;
				}
				else if (gotSign) { /* ..., ...	'+-'... ,... */
					printf("ERROR: multiple consecutive signs in file \"%s\" "
					"in line %d\n",argv,lineNum);
					error = 1;
				}
				else {
					gotSign = 1;
				}
			}
			else if (isdigit(ptr[i])) {
				gotNum = 1;
			}
			else if (ptr[i] == '\"' && type == STRUCT && !gotNum && !error) { /* add data and contiune to string */
				filterInt(linePtr,data_head);
				return count;
			}
			else { /* not a digit or a comma or a sign */
				if (type == STRUCT) {
					return -1; /* missing colon for string */
				}
				printf("ERROR: argument is not an integer in file \"%s\" in line %d\n",argv,lineNum);
				return 0;
			}
		} /* end of for */
		ptr = strtok(NULL," \t\n");
	} /* end of while */
	if (!gotNum) { /* string ended on a comma */
		if (count) {
			printf("ERROR: extraneous comma in file \"%s\" in line %d\n",argv,lineNum);
			error = 1;
		}
		else {
			printf("ERROR: missing argument in file \"%s\" in line %d\n",argv,lineNum);
			error = 1;
		}
	}
	if (error) {
		return 0;
	}
	filterInt(linePtr,data_head);
	return count + 1;
}

/* returns the number of string characters and adds data to the data's linked list */
int getStringLength(char *ptr, int lineNum, char *argv, DataList *data_head)
{
	int count;

	if (*ptr == '\"') { /* must start with a colon */
		ptr++;
		for (count = 0; ptr[count] != '\"'; count++) {
			if (!ptr[count] || ptr[count] == '\n') {
				printf("ERROR: in file \"%s\" in line %d:\n"
					"expected \" to complete the string.\n",argv,lineNum);
				return 0;
			}
			addData(ptr[count],data_head);
		}
		ptr += count + 1;
		ptr = skipWhiteSpaces(ptr);
		if (*ptr == '\n' || !*ptr) {
			addData('\0',data_head); /* add the null terminator character */
			return count + 1; /* the length of the string + the null terminator */
		}
		else {
			printf("ERROR: extraneous text after command in file \"%s\" in line %d.\n"
				,argv,lineNum);
			return 0;
		}
	}
	else { /* didn't start with a colon */
		printf("ERROR: invalid string in file \"%s\" in line %d.\n",argv,lineNum);
	}
	return 0;
}

/* uses getDataLength and getStringLength to calculate the number of words, adds their data to the data's list and
   return their combined number of words, return 0 if got an error in either of the functions */
int getStructLength(char *ptr, char *linePtr, int lineNum, char *argv, int type, DataList *data_head)
{
	int dataLength = 0, stringLength = 0, i = 0, error = 0;
	
	dataLength = getDataLength(ptr,linePtr,lineNum,argv,type,data_head);
	if (dataLength < 1){
		error = 1;
	}
	if (dataLength != -1) { /* starting colon exists */
		while (linePtr[i] != '\"') {
			if (linePtr[i] == '\n' || !linePtr[i]) { /* colon not found */
				break;
			}
			i++;
		}
		linePtr += i; /* skip to the start of the string */
	}
	stringLength = getStringLength(linePtr,lineNum,argv,data_head);
	if (!stringLength) {
		error = 1;
	}
	if (error) {
		return 0;
	}
	return dataLength + stringLength;
}

/* uses linePtr instead of the parsed line pointer to filter the integer from the file, and add
   him to the data's list. */
void filterInt(char *ptr, DataList *data_head)
{
	int i = 0, gotMinus = 0, data = 0;
	while (ptr[i] != '\"' && ptr[i] && ptr[i] != '\n') { 
		if (ptr[i] == '+' || ptr[i] == '-') {
			if (ptr[i] == '-') {
				gotMinus = 1;
			}
			ptr += i + 1; /* move the pointer to after the sign for atoi() */
			i = 0; /* reset the offset */
		}
		else if (ptr[i] == ',') {
			gotMinus ? (data = -(atoi(ptr))) : (data = atoi(ptr));
			addData(data, data_head);
			gotMinus = 0;
			ptr += i + 1; /* move the pointer to after the comma */
			i = 0; /* reset the offset */
		}
		else {
			i++;
		}
	}
	if (ptr[i] == '\"') { /* ..., ".."   nothing is supposed to be between a comma and a colon */
		return;
	}
	gotMinus ? (data = -(atoi(ptr))) : (data = atoi(ptr));
	addData(data, data_head);
}

















