
typedef struct {
	unsigned int ARE : 2;
	unsigned int value : 8;
	} Word; /* for the addressing purposes */

typedef struct {
	unsigned int ARE : 2;
	unsigned int dest : 4;
	unsigned int src : 4;
	} Register; /* for the register addressing */

/* converts the input to character of base 32, assuming the number is below 32.
   input: num = the number input.
   returns the matching base 32 character. */
char convertToBase32(int num);

/* prints the headline of the obj file, the number of instructions and the number of data.
   input: ic = instruction counter, dc = data counter.
   fp = output file pointer. */
void printHead(int ic, int dc, FILE *fp);

/* prints the address of the word.
   input: address = the address of the word.
   fp = the output file pointer */
void printAddress(int address, FILE *fp);

/* prints an instruction from the code's list.
   input: ptr = pointer to the code's list.
   fp = output file pointer. */
void printInstruction(CodeList *ptr, FILE *fp);

/* prints the addressing's word.
   input: input = the input word.
   address = the word's address.
   fp = output file pointer. */
void printWord(Word input, int address, FILE *fp);

/* prints the register addressing's word.
   input = the input word.
   address = the word address.
   fp = output file pointer. */
void printRegister(Register input, int address, FILE *fp);

/* prints the entry table.
   input: str = the symbol.
   head = symbol table's pointer.
   fp = output file pointer.
   lineNum,argv = for error if the error was already assigned as extern.
   returns 1 if successful, 0 if the label was already assigned as extern. */
int printEntry(char *str, SymbolTable *head, FILE *fp, int lineNum, char *argv);

/* prints the datalist.
   input: tmp_ic = the address.
   head = data list pointer.
   fp = output file pointer. */
void printDataList(int tmp_ic, DataList *head, FILE *fp);

