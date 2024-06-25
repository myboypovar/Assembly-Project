#include "symbol_table.h"
#include "decode.h"
#include "instructions.h"
#include "utility.h"
#include "output.h"

/* converts the integer to base 32 character */
char convertToBase32(int num)
{
	if (!num)
		return '!';
	if (num == 1)
		return '@';
	if (num == 2)
		return '#';
	if (num == 3)
		return '$';
	if (num == 4)
		return '%';
	if (num == 5)
		return '^';
	if (num == 6)
		return '&';
	if (num == 7)
		return '*';
	if (num == 8)
		return '<';
	if (num == 9)
		return '>';
	if (num > 9 && num < 32) { 
		return num + 87;	/* match the ascii chart */
	}
	return -1; /* num is bigger than 31 */
}

/* prints the headline that contains the ic and the dc. */
void printHead(int ic, int dc, FILE *fp)
{
	char numOfIns[3], numOfData[3];
	memset(numOfIns,'\0',3);
	memset(numOfData,'\0',3);
	if ((ic - MEMORY_START) > 31) { /* print to digits */
		numOfIns[0] = convertToBase32((ic - MEMORY_START) / 32);
		numOfIns[1] = convertToBase32((ic - MEMORY_START) % 32);
	} else {			/* print a single digit */
		numOfIns[0] = convertToBase32(ic - MEMORY_START);
	}
	if (dc > 31) {
		numOfData[0] = convertToBase32(dc / 32);
		numOfData[1] = convertToBase32(dc % 32);
	} else {
		numOfData[0] = convertToBase32(dc);
	}
	fprintf(fp,"    %s %s\n",numOfIns,numOfData);
}

/* prints the address */
void printAddress(int address, FILE *fp)
{
	char word[3];
	word[0] = convertToBase32(address / 32);
	word[1] = convertToBase32(address % 32);
	word[2] = '\0';
	fprintf(fp,"%s\t",word);
}

/* prints the current instruction */
void printInstruction(CodeList *ptr, FILE *fp)
{
	char word[3];
	int num = ptr -> ARE | ptr -> dest_addressing << 2 | ptr -> src_addressing << 4 | ptr -> op_code << 6;
	word[0] = convertToBase32(num / 32);
	word[1] = convertToBase32(num % 32);
	word[2] = '\0';
	fprintf(fp,"%s\n",word);
}

/* prints an addressing's word */
void printWord(Word input, int address, FILE *fp)
{
	char word[3];
	int num = input.ARE | input.value << 2;
	printAddress(address,fp);
	word[0] = convertToBase32(num / 32);
	word[1] = convertToBase32(num % 32);
	word[2] = '\0';
	fprintf(fp,"%s\n",word);
}

/* prints a register addressing's word */
void printRegister(Register input, int address, FILE *fp)
{
	char word[3];
	int num = input.ARE | input.dest << 2 | input.src << 6;
	printAddress(address,fp);
	word[0] = convertToBase32(num / 32);
	word[1] = convertToBase32(num % 32);
	word[2] = '\0';
	fprintf(fp,"%s\n",word);
}

/* prints the entry table */
int printEntry(char *str, SymbolTable *head, FILE *fp, int lineNum, char *argv)
{
	SymbolTable *ptr = head;
	if (head -> next == head) { /* no symbols */
		printf("ERROR: in file \"%s\" in line %d.\nthe label \"%s\" doesn't exist.\n",argv,lineNum,str);
		return 0;
	}
	while (ptr) { /* search for the label */
		if (!strcmp(ptr->symbolName, str)) {
			if (ptr -> type == EXTERN) { /* the label was already assigned as .extern */
				printf("ERROR: in file \"%s\" in line %d:\nthe label \"%s\" is assigned as \".extern\", "
					"therefore cannot\nbe assigned as \".entry\".\n",argv,lineNum,str);
				return 0;
			} else { /* print the symbol */
				char word[3];
				word[0] = convertToBase32(ptr -> address / 32);
				word[1] = convertToBase32(ptr -> address % 32);
				word[2] = '\0';
				fprintf(fp,"%s\t%s\n",str,word);
				return 1;
			}
		}
		ptr = ptr -> next;
	}
	printf("ERROR: in file \"%s\" in line %d.\nthe label \"%s\" doesn't exist.\n",argv,lineNum,str);
	return 0;
}

/* prints the data after the instructions */
void printDataList(int tmp_ic, DataList *head, FILE *fp)
{
	DataList *ptr = head;
	if (head -> next == head) { /* list empty */
		return;
	}
	while (ptr) {
		char word[3];
		int num = ptr -> data;
		printAddress(tmp_ic,fp);
		word[0] = convertToBase32((num >> 5) & 0x1F);
		word[1] = convertToBase32(num & 0x1F);
		word[2] = '\0';
		fprintf(fp,"%s\n",word);
		tmp_ic++;
		ptr = ptr -> next;
	}
}
