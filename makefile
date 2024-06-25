assembler: assembler.o macro.o first_pass.o symbol_table.o decode.o instructions.o utility.o second_pass.o output.o
	gcc -g -ansi -pedantic -Wall assembler.o macro.o first_pass.o symbol_table.o decode.o \
		instructions.o utility.o second_pass.o output.o -o assembler

assembler.o: assembler.c macro.h first_pass.h second_pass.h
	gcc -c -g -ansi -pedantic -Wall assembler.c -o assembler.o

macro.o: macro.c macro.h
	gcc -c -g -ansi -pedantic -Wall macro.c -o macro.o

first_pass.o: first_pass.c first_pass.h instructions.h utility.h 
	gcc -c -g -ansi -pedantic -Wall first_pass.c -o first_pass.o

symbol_table.o: symbol_table.c symbol_table.h utility.h
	gcc -c -g -ansi -pedantic -Wall symbol_table.c -o symbol_table.o

decode.o: decode.c decode.h utility.h
	gcc -c -g -ansi -pedantic -Wall decode.c -o decode.o

instructions.o: instructions.c instructions.h decode.h utility.h
	gcc -c -g -ansi -pedantic -Wall instructions.c -o instructions.o

utility.o: utility.c utility.h
	gcc -c -g -ansi -pedantic -Wall utility.c -o utility.o

second_pass.o: second_pass.c second_pass.h decode.h symbol_table.h instructions.h utility.h
	gcc -c -g -ansi -pedantic -Wall second_pass.c -o second_pass.o

output.o: output.c output.h decode.h symbol_table.h instructions.h utility.h
	gcc -c -g -ansi -pedantic -Wall output.c -o output.o
