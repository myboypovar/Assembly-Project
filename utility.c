#include "utility.h"

/* checks if the current parsed word is a register, expectedComma = 1 if there are 2 arguments */
int isRegister(char *ptr, int expectedComma)
{
	if (*ptr == 'r' && isdigit(ptr[1]) && ptr[1] != '8' && ptr[1] != '9') { /* r0 to r7 */
		if (ptr[2] == ':') { /* mark to an invalid label */
			return -1;
		}
		if (!ptr[2] || (ptr[2] == ',' && expectedComma)) { /* if there are 2 arguments */
			return ptr[1] - '0' + 1;  /* ptr[1] - '0' = integer that matches the the number */
		}
	}
	return 0;
}

/* checks if the current parsed word is an opcode */
int isOpCode(char *ptr)
{
	int i, n;
	char *opName[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","get","prn","jsr",
				"rts","hlt"};
	n = sizeof(opName) / (strlen(opName[0]) + 1); /* number of elements in the array */
	
	if (strlen(ptr) <= 4) {
		for (i=0; i < n; i++) {
			if (!strncmp(ptr, opName[i], 3)) {
				if (ptr[3] == ':') { /* for checkForLabel() */
					return -1;
				}
				if (!ptr[3]) {
					return i+1;
				}
			}
		}
	}
	return 0;
}

/* checks if the string is data or entry or extern instruction */
int checkForType(char *ptr, int lineNum, char *argv)
{
	int i;
	char *type[] = {".data", ".string", ".struct", ".entry", ".extern"};
	
	if (*ptr == '.') {
		for (i=0; i < TYPE_NUM; i++) {
			if (!strcmp(ptr, type[i])) {
				return i+1;
			}
		}
		printf("ERROR: guidance name doesn't exist in file \"%s\" in line %d\n",argv,lineNum);
		return -1;
	}
	return 0;
}

/* skips white spaces */
char *skipWhiteSpaces(char *ptr)
{
	int i;
	for (i=0; ptr[i] == ' ' || ptr[i] == '\t'; i++);
	return ptr + i;
}

/* skips to the data */
char *skipToData(char *ptr, int label)
{
	ptr = skipWhiteSpaces(ptr);
	if (label) { /* skip the label name */
		ptr = skipWord(ptr);
	}
	ptr = skipWhiteSpaces(ptr);
	ptr = skipWord(ptr); /* skip the data type name */
	ptr = skipWhiteSpaces(ptr);
	return ptr;
}

/* skips a single word */
char *skipWord(char *ptr)
{
	int i;
	for (i=0; ptr[i] != ' ' && ptr[i] != '\t'; i++);
	return ptr + i;
}

/* malloc function */
void *mallocCheck(int size)
{
	void *ptr;
	if (!(ptr = malloc(size))) {
		printf("ERROR: memory allocation failed");
		exit(0);
	}
	return ptr;
}

/* mix between malloc and strcat for the file name */
char *strmalloccat(char *argv, char *str)
{
	char *ptr;
	ptr = (char *)mallocCheck(strlen(argv) + strlen(str) + 1);
	strcpy(ptr,argv);
	strcat(ptr,str);
	return ptr;
}


