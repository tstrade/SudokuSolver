DEPS := Soduku.c Soduku.h CSP.c CSP.h Solver.c Solver.h datastructs.c datastructs.h
CC := gcc
FLAGS := -Wall -Werror -std=c99

test: $(DEPS)
	$(CC) $(FLAGS)  Soduku.c -o Soduku.o
	./Soduku.o 12.456789.........12.45678912.45678912.45678912.45678912.45678912.45678912.456789

clean: Soduku.o
	rm -f Soduku.o
