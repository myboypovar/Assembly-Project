#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MEMORY_START 100 
#define LINE_LENGTH 80 /* assumption from the project's instructions */
#define TYPE_NUM 5  /* how many types for enum type */

enum type {DATA = 1, STRING = 2, STRUCT = 3, ENTRY = 4, EXTERN = 5, CODE = 6};

enum opCode {MOV = 1, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, GET, PRN, JSR,
		RTS, HLT};

/* checks if the pointer to the parsed line is a register.
   input: ptr = pointer to the parsed line, expectedComma = number of the arguments( =1 if there are 2 arguments)
   returns the register's number or if its a label return -1 else if its not a register returns 0 */
int isRegister(char *ptr, int expectedComma);

/* checks if the pointer to the parsed line is an op code.
   input: ptr = pointer to the parsed line.
   return the number of the opcode, if its an opcode label returns -1, else if its not an opcode returns 0. */
int isOpCode(char *ptr);

/* checks if the string is a guidance or extern or entry.
   input: ptr = pointer to the parsed line, lineNum,argv = for error.
   return 1 if data, 2 if string, 3 if struct.
   returns -1 if guidance does not exist, and 0 if not a guidance (instruction) */
int checkForType(char *ptr, int lineNum, char *argv);

/* skips spaces.
   input: ptr = pointer to the line.
   returns a pointer next word in the line. */
char *skipWhiteSpaces(char *ptr);

/* skips to the data.
   input: ptr = pointer to the line, label = if there was a label (to know if there is a need to skip it).
   return a pointer in the line. */
char *skipToData(char *ptr, int label);

/* skips a single word.
   input: ptr = pointer to the line.
   returns a pointer to the line. */
char *skipWord(char *ptr);

/* checks if malloc succeded.
   input: size = the size of the malloc in bytes
   returns a void pointer. */
void *mallocCheck(int size);

/* allocates memory for the file name.
   input: argv = the file's name, str = the suffix.
   returns a pointer to the string */
char *strmalloccat(char *argv, char *str);

