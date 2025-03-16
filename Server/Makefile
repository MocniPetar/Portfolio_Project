CC = /usr/bin/gcc
CFLAGS = -Wall -O2
TARGETS = start_server clean

default: all
all: $(TARGETS)

start_server: Server.o
	@echo "Linking..."
	@$(CC) $(CFLAGS) WebSiteClient.c Server.c -o run_server -lcurl

%.o:%.c
	@echo "Compiling..."
	@$(CC) $(CFLAGS) -c $< 

clean:
	rm -f *.o
