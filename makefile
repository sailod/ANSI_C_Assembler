CC=gcc
CFLAGS=-Wall -ansi -pedantic

OBJECTS= main.o directives.o error_handling.o file_processing.o keywords.o opcode.o symbol_tree.o extern_table.o machine_code.o

assembler:  $(OBJECTS)
	$(CC) $(CFLAGS) -g $(OBJECTS)  -o assembler

directives.o: directives.c directives.h
	$(CC) $(CFLAGS) -c directives.c -o directives.o

error_handling.o: error_handling.c error_handling.h
	$(CC) $(CFLAGS) -c error_handling.c -o error_handling.o

file_processing.o: file_processing.c file_processing.h
	$(CC) $(CFLAGS) -c file_processing.c -o file_processing.o

keywords.o: keywords.c keywords.h
	$(CC) $(CFLAGS) -c keywords.c -o keywords.o

opcode.o: opcode.c opcode.h
	$(CC) $(CFLAGS) -c opcode.c -o opcode.o

symbol_tree.o: symbol_tree.c symbol_tree.h
	$(CC) $(CFLAGS) -c symbol_tree.c -o symbol_tree.o

machine_code.o: machine_code.c machine_code.h
	$(CC) $(CFLAGS) -c machine_code.c -o machine_code.o

extern_table.o: extern_table.c extern_table.h
	$(CC) $(CFLAGS) -c extern_table.c -o extern_table.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o


clean: 	
	rm -f assembler
	rm -f *.o
	rm -f example.ob
	rm -f example.ent
	rm -f example.ext
	rm -f example2.ob
	rm -f example2.ent
	rm -f example2.ext
