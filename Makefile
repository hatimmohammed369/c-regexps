run :
	@echo "Compiling . . . "
	gcc -std=gnu11 -o main.o main.c lib.h scanner/tokens.h scanner/scanner.h\
	 scanner/scanner.c
	@echo
	@echo "Running . . . "
	@./main.o
