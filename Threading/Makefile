DEPS := Threaded_Soduku.c Threaded_Soduku.h CSP.c CSP.h Threaded_Solver.c Threaded_Solver.h datastructs.c datastructs.h
CC := gcc
CFLAGS := -g -Wall -Werror -std=c99
DB := gdb
DFLAGS := -args
COMP := Threaded_Soduku.c -o Threaded_Soduku.o
EX := Threaded_Soduku.o
ARG1 := 12.456789.........12.45678912.45678912.45678912.45678912.45678912.45678912.456789
ARG2 := ..3.2.6..9..3.5..1..18.64....81.29..7.......8..67.82....26.95..8..2.3..9..5.1.3..
ARG3 := ...7.46.3..38...51.1.9.327..34...76....6.8....62...98..473.6.1.68...13..3.12.5...
ARG4 := ..5...1.3....2.........176.7.49....1...8.4...3....7..8.3.5....2....9....4.6...9..
ARG5 := 8..........36......7..9.2...5...7.......457.....1...3...1....68..85...1..9....4..

compile: $(DEPS)
	$(CC) $(CFLAGS) $(COMP)

easyTest: compile
	./$(EX) $(ARG1)

mediumTest: compile
	./$(EX) $(ARG2)

hardTest: compile
	./$(EX) $(ARG3)

inkalaTest: compile
	./$(EX) $(ARG4)

evilTest: compile
	./$(EX) $(ARG5)

debug: compile
	$(DB) $(DFLAGS) ./$(EX) $(ARG)

clean: $(EX)
	rm -f $(EX)
