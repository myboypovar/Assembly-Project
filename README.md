# Assembly Language Simulator in C

## Project Description
This project is a C program designed to simulate the operations of assembly language. The program accepts command line arguments written in a pseudo-assembly language and executes these instructions as if they were processed by an actual assembler. This simulation provides an educational tool for understanding how assembly language works, implemented in C.

Implementation Stages:

1) Macros stage:
The assembler parses the original source (.as) file and generates the expanded (.am) source file that contains the same source code content, the only difference is that the macros defined in the original source file (.as) are replaced with their content. To do so the assembler uses a hash table (Macro Table) to store the macro name with its starting and ending indexes (FILE position) of its content. If it finds any error in the syntax of the macros it yields a relevant error message and jumps straight to Next, otherwise, it continues to the First Run stage.

2) First Run:
In the first run, the assembler counts the size of the data image and the instruction image (without allocating any memory), adds all the symbols to the symbol table, and last but not least the assembler verifies that there are no errors in code before it moves on to step 3. If the assembler encounters any error in the code it outputs an error/warning message to stderr and error.log (file which will be created in the root folder if any errors/warnings occur). To report any error in the source code in the first run, the assembler parses all the content of the source code regardless of any occurrence of an error. If any error (warnings do not count as errors) occurs, the assembler will not continue to stage 3 but it will go straight on to the Next stage.

3) In between the first and second run:
If the first run ended successfully (else: Next) the assembler will update the address of each data symbol in the symbol table, and then it will allocate the exact amount of memory it needs to write for the target image (binary of compiled code) then it will reset the counters (IC/DC) and continue to the second run.

5) Second run:
In the second run, our assembler writes the words of each line of code in binary format, inserts the words (binary bits) to the memory image in the correct position depending on the type of word (data/instruction) and adds each external operand that appeared in the correct spot in the external operands table.
If the assembler encounters a label operand that is not within the symbol table and is not external it yields an error message and then continues to check the rest of the code to discover all errors of this kind and report them (it is the only kind of error that can appear in the second run stage since any other type of error already been reported in the previous stages by now and has already prevented the assembler from entering the second run stage) and in this case, the assembler will finish the second run but will not export any files.

5) Export (generate files):
If the second run is finished without any error (else: Next) we will generate all of the required outputs (.ob, .ext .ent files).
