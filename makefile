assembler:	main.o handler.o parser.o errors.o symbol_table.o memory_image.o
	gcc -g -ansi -pedantic -Wall errors.o main.o handler.o parser.o symbol_table.o memory_image.o -o assembler
main.o:	main.c main.h
	gcc -c -ansi -pedantic -Wall main.c -o main.o
handler.o:	handler.c handler.h
	gcc -c -ansi -pedantic -Wall handler.c -o handler.o
parser.o:	parser.c parser.h
	gcc -c -ansi -pedantic -Wall parser.c -o parser.o
errors.o:	errors.c errors.h
	gcc -c -ansi -pedantic -Wall errors.c -o errors.o
symbol_table.o:	symbol_table.c symbol_table.h
	gcc -c -ansi -pedantic -Wall symbol_table.c -o symbol_table.o
memory_image.o:	memory_image.c memory_image.h
	gcc -c -ansi -pedantic -Wall memory_image.c -o memory_image.o
