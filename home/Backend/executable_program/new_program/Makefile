CC = /usr/bin/gcc
CFLAGS = -Wall
TARGETS = exercise_tester clean

default: all
all: $(TARGETS)

exercise_tester: exerciseTester.c
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o test -lseccomp
	@echo "Running..."
	./test

%.o: %.c
	@echo "Compiling..."
	@$(CC) $(CFLAGS) -c $<

clean:
	@rm -f *.o *~ test
