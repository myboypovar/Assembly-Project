#include "decode.h"
#include "utility.h"

/* adds code word to the code's list */
void addCode(int opCode,int srcAdd, int destAdd, CodeList *head)
{
	CodeList *ptr = head;
	CodeList *newCode;
	if (head -> next == head) { /* list is empty */
		head -> ARE = 0;
		head -> dest_addressing = destAdd;
		head -> src_addressing = srcAdd;
		head -> op_code = opCode;
		head -> next = NULL;
		return;
	}
	while (ptr) {
		if (ptr -> next == NULL) {
			break;
		}
		ptr = ptr -> next;
	}
	newCode = (CodeList *)mallocCheck(sizeof(CodeList));
	ptr -> next = newCode;
	newCode -> ARE = 0; /* always 0 */
	newCode -> dest_addressing = destAdd;
	newCode -> src_addressing = srcAdd;
	newCode -> op_code = opCode;
	newCode -> next = NULL;
}

/* add data to the data's list */
void addData(int input, DataList *head)
{
	DataList *ptr = head;
	DataList *newData;
	if (head -> next == head) {
		head -> data = input;
		head -> next = NULL;
		return;
	}
	while (ptr) {
		if (ptr -> next == NULL) {
			break;
		}
		ptr = ptr -> next;
	}
	newData = (DataList *)mallocCheck(sizeof(DataList));
	ptr -> next = newData;
	newData -> data = input;
	newData -> next = NULL;
}

/* print the data list in binary */
void printData(DataList *head)
{
	DataList *ptr = head;
	int i, mask = 512, data_ptr = 0; /* mask = the msb */
	if (head -> next == head) {
		return; /* empty list */
	}
	while (ptr) {
		puts("\n");
		data_ptr = ptr -> data;
		for (i=0; i < 10; i++) {
			data_ptr & mask ? putchar('1') : putchar('0');
			
			data_ptr <<= 1;
		}
		ptr = ptr -> next;
	}
	puts("\n");
}

/* frees the code and data list */
void freeDecode(CodeList *code_head, DataList *data_head)
{
	CodeList *cptr = code_head;
	DataList *dptr = data_head;
	if (cptr -> next == code_head) {
		free(cptr);
		cptr = NULL;
	}
	if (dptr -> next == data_head) {
		free(dptr);
		dptr = NULL;
	}
	while (1) {
		CodeList *ctmp;
		DataList *dtmp;
		if (cptr) {
			ctmp = cptr -> next;
			free(cptr);
			cptr = ctmp;
		}
		if (dptr) {
			dtmp = dptr -> next;
			free(dptr);
			dptr = dtmp;
		}
		if (!cptr && !dptr) { /* both empty */
			return;
		}
	}
}






