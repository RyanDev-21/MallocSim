CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb

heap : main.c
	@$(CC) $(CFLAGS) -o heap main.c
run : main.c
	@$(CC) $(CFLAGS) -o heap main.c && ./heap

clean: 
	rm -rf heap
