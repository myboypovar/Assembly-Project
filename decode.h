
typedef struct Cw {
	unsigned int ARE : 2;
	unsigned int dest_addressing : 2; /* destination addressing */
	unsigned int src_addressing : 2; /* source addressing */
	unsigned int op_code : 4;
	struct Cw *next;
	} CodeList;

typedef struct Dw {
	int data : 10; /* int for complement 2 */
	struct Dw *next;
	} DataList;

/* add code to the code's list.
   input: opCode = the opcode index.
   srcAdd = source addressing, destAdd = destination addressing.
   head = code list pointer. */ 
void addCode(int opCode,int srcAdd, int destAdd, CodeList *head);

/* add data to the data's list.
   input: input = the data integer, if char works with the ascii table.
   head = pointer to the data list. */
void addData(int input, DataList *head);

/* print the data list */
void printData(DataList *head);

/* frees both the code and the data lists.
   input: code_head = code list pointer, data_head = data list pointer. */
void freeDecode(CodeList *code_head, DataList *data_head);

