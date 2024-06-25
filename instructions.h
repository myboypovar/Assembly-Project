
enum addressing {IMMEDIATE, DIRECT, RECORD_ACCESS, REGISTER}; /* addressing types */

/* proccesses the instruction and adds the instruction to the code's list.
   input: ptr = pointer to a parsed line, opCode = the opcode index.
   argv, lineNum = for error
   head = pointer to the code's list.
   returns the number of words from the instruction, if error returns 0. */
int handleInstruction(char *ptr, int opCode, int lineNum, char *argv, CodeList *head);

/* returns the number of the expected arguments for the opcode */
int numOfArguments(int opCode);

/* checks whether the argmunets addressing type is immediate, direct, record-access or register.
   input: ptr = pointer to the parsed line.
   lineNum,argv = for error.
   expectedComma = if there are 2 arguments, expectedComma = 1 else 0.
   returns the addressing type index, if error returns -1.*/
int addressingType(char *ptr, int lineNum, char *argv, int expectedComma);

/* checks if the addressing types of the arguments are correct.
   input: opCode = opcode.
   addressing = the addressing of the argument.
   argNum = which argument it is, 0 for source, 1 for destination.
   lineNum,argv = for error.
   return 1 if the addressing is correct else returns 0. */
int checkAddressing(int opCode, int addressing, int argNum, int lineNum, char *argv);

/* input: addressing = the addressing type of the operand.
   returns the number of words from that addressing. */
int getWordsNum(int addressing);

