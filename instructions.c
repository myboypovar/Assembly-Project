#include "decode.h"
#include "instructions.h"
#include "utility.h"

/* adds code to the code's list and returns the number of code words from the instruction */
int handleInstruction(char *ptr, int opCode, int lineNum, char *argv, CodeList *head)
{
	/* words = number of words from the instruction, argumens = num of arguments, srcAdd = source addressing
           destAdd = destination addressing. */
	int error = 0, words = 1, arguments, srcAdd, destAdd;
	
	arguments = numOfArguments(opCode);
	if (!arguments) {
		if ((ptr = strtok(NULL," \t\n"))) {
			printf("ERROR: extraneous text after command in file \"%s\" in line %d.\n",argv,lineNum);
			return 0;
		}
		addCode(opCode-1,0,0,head);
		return 1;
	}
	if (!(ptr = strtok(NULL," \t\n"))) {
		printf("ERROR: missing arguments in file \"%s\" in line %d.\n",argv,lineNum);
		return 0;
	}
	
	if (arguments == 1) {
		destAdd = addressingType(ptr,lineNum,argv,0);
		if ((ptr = strtok(NULL," \t\n"))) { /* if the line is not empty after the argument */
			printf("ERROR: extraneous text after command in file \"%s\" in line %d.\n",argv,lineNum);
			error = 1;
		}
		if (destAdd == -1) {
			error = 1;
		} else {
			if (checkAddressing(opCode,destAdd,1,lineNum,argv)) {
				addCode(opCode-1,0,destAdd,head); /* one argument = destination addressing */
				words += getWordsNum(destAdd);
			} else {
				return 0;
			}
		}
	} else { /* 2 arguments */
		int n, expectedComma = 1, comma = 0;
		srcAdd = addressingType(ptr,lineNum,argv,1);
		for (n=0; ptr[n]; n++) {
			if (ptr[n] == ',' && ptr[n+1]) { /* if the string contains both arguments */
				ptr += n + 1; /* skip to after the comma to get the second argument */
				comma++;	/* add to the total number of commas */
				expectedComma = 0; /* got 1 comma, don't need another*/
				n = 0; /* reset the offset */
			}
			else if (ptr[n] == ',') {
				comma++;
			}
			if (comma > 1) { /* more than 1 comma */
				printf("ERROR: too many arguments in file \"%s\" in line %d.\n",argv,lineNum);
				return 0;
			}
			
		}
		if (expectedComma) { /* if the string doesn't contain a comma */
			ptr = strtok(NULL," \t\n");
			if (!ptr) {
				printf("ERROR: missing destination argument. in file \"%s\" in line %d.\n",argv,lineNum);
				return 0;
			}
			if (!comma) { /* no comma found in the first string */
				if (*ptr == ',' && !ptr[1]) { /* if the string contains only a comma */
					comma++;
					if (!(ptr = strtok(NULL," \t\n"))) {
						printf("ERROR: missing destination argument. "
							"in file \"%s\" in line %d.\n",argv,lineNum);
						return 0;
					}
				}
				else if (*ptr != ',') { /* missing comma */
					printf("ERROR: missing comma. in file \"%s\" in line %d.\n",argv,lineNum);
					return 0;
				}
				if (!comma) { /* comma included at the start of the 2nd argument.*/
					ptr++; /* move the pointer to after the comma */
				}
			}
		}
		destAdd = addressingType(ptr,lineNum,argv,0);
		if ((ptr = strtok(NULL," \t\n"))) {
			printf("ERROR: extraneous text after command in file \"%s\" in line %d.\n",argv,lineNum);
			error = 1;
		}
		if (srcAdd == -1 || destAdd == -1) {
			error = 1;
		} else {
			int src, dest;
			src = checkAddressing(opCode,srcAdd,0,lineNum,argv);
			dest = checkAddressing(opCode,destAdd,1,lineNum,argv);
			if (src && dest) {
				addCode(opCode-1,srcAdd,destAdd,head);
				if (srcAdd == REGISTER && destAdd == REGISTER) {
					words++;  /* both arguments are registers = shared word. */
				} else {
					words+= (getWordsNum(srcAdd) + getWordsNum(destAdd));
				}
			} else { /* one of the addressings is invalid */
				return 0;
			}
		}
	}
	if (error) {
		return 0;
	}
	return words;
}
/* returns the expected number of the arguments */
int numOfArguments(int opCode)
{
	if (opCode < NOT || opCode == LEA) {
		return 2;
	}
	if ((opCode > LEA && opCode < RTS) || opCode == NOT || opCode == CLR) {
		return 1;
	} else {
		return 0;
	}
}

/* checks whether the argmunets addressing type is immediate, direct, record-access or register.*/
int addressingType(char *ptr, int lineNum, char *argv, int expectedComma)
{
	int i, n; /* n for the length of the first argument */

	for (n=0; ptr[n]; n++) { /* calculate the argument's length */
		if (ptr[n] == ',' && expectedComma) { /* first argument length */ 
			break;
		}
		else if (ptr[n] == ',' && !expectedComma) {
			printf("ERROR: extraneous text after command in file \"%s\" in line %d.\n",argv,lineNum);
			return -1;
		}
	} 
	if (*ptr == '#') {
		if (ptr[1] == '+' || ptr[1] == '-' || isdigit(ptr[1])) {
			for (i=2; i < n; i++) {
				if (!isdigit(ptr[i])) {
					printf("ERROR: argument is not an integer in file \"%s\" "
						"in line %d.\n",argv,lineNum);
					return -1;
				}
			}
			return IMMEDIATE;
		}
	}
	if (isRegister(ptr,expectedComma) > 0) {
		return REGISTER;
	}
	if (isalpha(*ptr)) {
		for (i = 1; i < n; i++) {  /* i+2 == n to mark that there is no more text after the struct field */
			if (ptr[i] == '.' && (ptr[i+1] == '1' || ptr[i+1] == '2') && (i+2 == n)) {
				return RECORD_ACCESS; /* struct name in argument */
			}
			if (!isalnum(ptr[i])) {
				printf("ERROR: invalid argument in file \"%s\" in line %d.\n",argv,lineNum);
				return -1;
			}
		}
		return DIRECT; /* label name in argument */
	} else {
		printf("ERROR: invalid argument in file \"%s\" in line %d.\n",argv,lineNum);
			return -1;
	}
}

/* checks if the instruction can be compiled, if the addressing type of the arguments are legal */
int checkAddressing(int opCode, int addressing, int argNum, int lineNum, char *argv)
{
	if ((opCode >= MOV && opCode <= SUB) && !argNum) {
		return 1;
	}
	if (opCode == LEA && !argNum) {
		if (addressing == DIRECT || addressing == RECORD_ACCESS) {
			return 1;
		} else {
			printf("ERROR: illegal source argument, argument must be a label.\nin file \"%s\" "
				"in line %d.\n",argv,lineNum);
			return 0;
		}
	}
	if ((opCode == CMP || opCode == PRN) && argNum) {
		return 1;
	}
	if ((opCode != CMP && opCode != PRN) && argNum) {
		if (addressing != IMMEDIATE) {
			return 1;
		} else {
			printf("ERROR: illegal destination argument, argument must be a label or a register."
				"\nin file \"%s\" in line %d.\n",argv,lineNum);
			return 0;
		}		
	}
	return 0;
}

/* returns how many code words you get from an addressing */
int getWordsNum(int addressing)
{
	if (addressing == RECORD_ACCESS) {
		return 2;
	} else {
		return 1;
	}
}









