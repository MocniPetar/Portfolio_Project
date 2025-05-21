CC = /usr/bin/gcc
CFLAGS = -Wall -O2 -fsanitize=address -g -lcurl
TARGETS = start_server clean

default: all
all: $(TARGETS)

start_server: components/main.o
	@echo "Linking..."
	@$(CC) $(CFLAGS) components/main.c components/server_starting.c components/server_response.c components/important_components.c -o run_server

%.o:%.c
	@echo "Compiling..."
	@$(CC) $(CFLAGS) -c $<

clean:
	@echo "Cleaning folder..."
	@rm -f *.o
	@echo "Starting the server..."
	./run_server ../website_build/build