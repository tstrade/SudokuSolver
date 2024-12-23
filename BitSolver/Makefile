DEPS := Soduku.c Soduku.h CSP_Bits.c CSP_Bits.h ABitsC3.c ABitsC3.h qbit.c qbit.h
CC := gcc
CFLAGS := -g -Wall -Werror -std=c99 -lm
DB := gdb
DFLAGS := -args
COMP := Soduku.c -o Soduku.o
EX := Soduku.o
ARG1 := ..3.2.6..9..3.5..1..18.64....81.29..7.......8..67.82....26.95..8..2.3..9..5.1.3..
ARG2 := ...7.46.3..38...51.1.9.327..34...76....6.8....62...98..473.6.1.68...13..3.12.5...
ARG3 := ..5...1.3....2.........176.7.49....1...8.4...3....7..8.3.5....2....9....4.6...9..
ARG4 := 8..........36......7..9.2...5...7.......457.....1...3...1....68..85...1..9....4..
ARG5 := .7...2.686..5...13......7....6...5..4..........2.7.8.......14.7..32.6....1...5...
ARG6 := 123456789.........123456789123456789123456789123456789123456789123456789123456789

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

impossible: compile
	./$(EX) $(ARG6)

debug: compile
	$(DB) $(DFLAGS) ./$(EX) $(ARG1)

clean: $(EX)
	rm -f $(EX)
