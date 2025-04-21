CC = /usr/bin/gcc
CFLAGS = -Wall
TARGETS = task_tester clean

default: all
all: $(TARGETS)

task_tester: task_tester.o checking_program.o
	@echo "Linking..."
	$(CC) $(CFLAGS) $^ -o TaskTester

%.o: %.c
	@echo "Compiling..."
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *~ a.out
