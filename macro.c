#include "macro.h"

/* returns 1 if the macro name is invalid or if the file doesn't exist, returns 0 if successfuly created the ".am" file */
int readFile(char *argv)
{
	FILE *fpr, *fpw; /* fpr for reading the .as file, fpw to write the .am file */
	char *file_read = strmalloccat(argv,".as");
	char *file_write = strmalloccat(argv,".am");
	Macro *head = (Macro *)mallocCheck(sizeof(Macro));
	int error = 0;
	head -> next = head; /* mark that shows that the list is empty */
	if (!(fpr = fopen(file_read,"r"))) {
		printf("ERROR: the file \"%s\" does not exist.\n",argv);
		free(file_write);
		freeMacroList(head);
		return 1;
	}
	free(file_read);
	fpw = fopen(file_write,"w");
	if (checkForMacro(fpr, head, fpw, argv)) { /* if the macro name is invalid */
		remove(file_write); 
		error = 1;
	}
	free(file_write);
	freeMacroList(head);
	fclose(fpr);
	fclose(fpw);
	return error;
}

/* the function reads the input line and checks if a macro exists, and calls to an output function
   return 0 if successful , returns 1 if macro name is invalid */
int checkForMacro(FILE *fpr, Macro *head, FILE *fpw, char *argv)
{
	char line[LINE_LENGTH]; /* the line for fgets, line would get copied to macro's content if needed */
	char *ptr, *tmp; /* pointer for the line */
	int lineNum = 0, gotMacro = 0, i = 0, didMacro = 0;
	Macro *prevItem = head; /* macro's pointer */

	while (fgets(line,LINE_LENGTH,fpr)) {
		char tmpLine[LINE_LENGTH]; /* temporary line for parsing */
		Macro *item; /* new macro name if needed */
		lineNum++;
		strcpy(tmpLine,line);
		if (!(ptr = strtok(tmpLine, " \t\n")) || *ptr == ';') { /*to get only the first word without any white spaces */
			fputs(line,fpw);
			continue;
		}
		if (prevItem -> next != head) {
			didMacro = writeMacro(ptr,fpw,head); /* if encountered a macro's name */
		}
		
		if (!strcmp(ptr,"macro") && !gotMacro) { /* encountered the word "macro" */ 
			if (!(ptr = strtok(NULL," \t\n"))) { /* ignore macro without name */
				continue;
			}
			tmp = ptr;
			if ((ptr = strtok(NULL, " \t\n"))) { /* more than 1 name for the macro */
				printf("ERROR: Invalid macro name in file \"%s\" in line: %d.\n",argv,lineNum);
				return 1;
			}
			if (prevItem -> next == head) { /*the first argument in the macro's linked list */
				strcpy(head->macro_name,tmp);
				prevItem -> next = NULL;
				gotMacro = 1;
			}
			else { /* make new Macro node to link to the previous item */
				item = (Macro *)mallocCheck(sizeof(Macro));
				strcpy(item->macro_name,tmp);
				prevItem -> next = item;
				item -> next = NULL;
				prevItem = item;
				gotMacro = 1;
			}
		}
		else if (gotMacro && strcmp(ptr,"endmacro")) { /* copying lines to the macro's content */
			char *macro_line = (char *)mallocCheck(strlen(line) + 1);
			strcpy(macro_line,line);
			prevItem -> content[i] = macro_line;
			i++;
		}
		else if (gotMacro && !(strcmp(ptr,"endmacro"))) { /* endmacro encoutered */
			prevItem -> content[i] = NULL; /* mark to know where the macro's content ends */
			gotMacro = 0; /* reset to find another macro */
			i = 0;
		}
		else if (!didMacro) { /* to not copy the macro content while scanning */
			fputs(line,fpw); /* copy the line to the next flie */
		}
		
	}
	return 0;
}
/* implements the macro to the ".am" file, returns 1 if the function implemented the macro, if not returns 0. */
int writeMacro(char *ptr, FILE *fpw, Macro *head)
{
	int i = 0, didMacro = 0;
	Macro *itemPtr = head;
	while (itemPtr != NULL) {
		if (!strcmp(itemPtr -> macro_name, ptr)) { /* found the macro name */
			didMacro = 1;
			while (itemPtr -> content[i] != NULL) { 
				fputs(itemPtr -> content[i],fpw); /* write the macro on the .am file */
				i++;
			}
		}
		itemPtr = itemPtr -> next;
	}
	return didMacro;
}

/* frees the Macro's linked list */
void freeMacroList(Macro *head) /* free the macro list from malloc */
{
	int i = 0;
	Macro *ptr = head;
	Macro *tmp;
	if (head -> next == head) { /* mark for empty list */
		free(head);
		return;
	}
	while (ptr) {
		while (ptr->content[i]) { /* free each line of content */
			free(ptr->content[i]);
			i++;
		}
		tmp = ptr -> next;
		free(ptr);
		ptr = tmp;
	}
}









