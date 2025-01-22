A_CFL := AC3/AC3.c AC3/protections.c
ADEPS := AC3/AC3.h $(A_CFL)

C_CFL := CSP/binit.c CSP/assign.c CSP/display.c CSP/nconflicts.c CSP/prune.c
CDEPS := CSP/CSP.h $(C_CFL)

Q_CFL := Queue/qinit.c Queue/dequeue.c Queue/enqueue.c Queue/ispresent.c 
QDEPS := Queue/queue.h $(Q_CFL)

T_CFL := sudoku_threads.c
TDEPS := sudoku_threads.h $(T_CFL)

CFLS := $(T_CFL) $(Q_CFL) $(C_CFL) $(A_CFL)
DEPS := $(ADEPS) $(CDEPS) $(QDEPS) $(TDEPS)

CC := gcc
CFLAGS := -g -Wall -Werror -std=gnu99 -pthread

DB := gdb
DFLAGS := -args
EX := solver.o
COMP := $(CFLS) -o $(EX)

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
	$(DB) $(DFLAGS) ./$(EX) $(ARG2)

clean: $(EX)
	rm -f $(EX)
