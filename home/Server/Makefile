CC = /usr/bin/gcc
CFLAGS = -Wall -O2 -fsanitize=address -g
TARGETS = start_server clean

default: all
all: $(TARGETS)

start_server: Server.o
	@echo "Linking..."
	@$(CC) $(CFLAGS) Server.c -o run_server -lcurl

%.o:%.c
	@echo "Compiling..."
	@$(CC) $(CFLAGS) -c $< 

clean:
	@echo "Cleaning folder..."
	@rm -f *.o
	@echo "Starting the server..."
	./run_server ../WebSite/src