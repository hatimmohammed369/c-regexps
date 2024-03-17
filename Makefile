CC = gcc
CFLAGS = -Wall -Wextra -g -std=gnu11

test : compile
	@echo
	@echo "Running . . . "
	@./test

compile :
	@echo "Compiling . . . "
	@echo
	@echo "Source files:"
	@find -type f -regex "^./.*[.]\(c\|h\)$$"
	@echo
	$(CC) $(CFLAGS) -o test `find -type f -regex "^./.*[.]\(c\|h\)$$"`
