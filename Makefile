CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wtype-limits -pedantic
EXEC = pruebas

compile: main.c pruebas_catedra.c hash.c hash.h lista.c lista.h testing.c testing.h
	$(CC) $(CFLAGS) main.c pruebas_catedra.c hash.c lista.c testing.c -o $(EXEC)

run: compile
	./$(EXEC)

valgrind: compile
	valgrind ./$(EXEC) --track-origins=yes --leak-cheak=full

gdb: compile
	gdb ./$(EXEC) -tui
