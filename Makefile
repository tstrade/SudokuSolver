DEPS := Soduku.c Soduku.h CSP.c CSP.h Solver.c Solver.h datastructs.c datastructs.h
CC := gcc
CFLAGS := -g -Wall -Werror -std=c99
DB := gdb
DFLAGS := -args
COMP := Soduku.c -o Soduku.o
EX := Soduku.o
ARG := 12.456789.........12.45678912.45678912.45678912.45678912.45678912.45678912.456789

compile: $(DEPS)
	$(CC) $(CFLAGS) $(COMP)

test: compile
	./$(EX) $(ARG)

debug: compile
	$(DB) $(DFLAGS) ./$(EX) $(ARG)

clean: $(EX)
	rm -f $(EX)
