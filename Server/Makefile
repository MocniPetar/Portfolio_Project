CC = /usr/bin/gcc
CFLAGS = -Wall -O2 -fsanitize=address -g
TARGETS = start_server clean

default: all
all: $(TARGETS)

start_server: Server.o
	@echo "Linking..."
	@$(CC) $(CFLAGS) Server.c -o run_server -lcurl
	@mv run_server ./executable
	@echo "You can run the server by navigating to the executable folder and run ./run_server ../Website/main in the terminal..."

%.o:%.c
	@echo "Compiling..."
	@$(CC) $(CFLAGS) -c $< 

clean:
	@echo "Cleaning folder..."
	@rm -f *.o
