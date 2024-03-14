run :
	@echo "Compiling . . . "
	gcc -o main.o main.c lib.h scanner/tokens.h scanner/scanner.h
	@echo
	@echo "Running . . . "
	@./main.o
