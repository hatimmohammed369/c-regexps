compile :
	@echo "Compiling . . . "
	gcc -g -std=gnu11 -o main.o main.c lib.h \
		scanner/tokens.h scanner/tokens.c \
		scanner/scanner.h scanner/scanner.c

run : compile
	@echo
	@echo "Running . . . "
	@./main.o
