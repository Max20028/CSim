CC = clang
#Removed -municode from cflags
#Look into how to build .o to other folder
CFLAGS = -g -std=c11

all: output


output: main
	$(CC) $(CFLAGS) -o blank.exe main.o arena.o renderer.o

main: arena renderer
	$(CC) $(CFLAGS) -c main.c

renderer: arena
	$(CC) $(CFLAGS) -o renderer.o -c Engine/DX11Renderer.c

arena:
	$(CC) $(CFLAGS) -c Engine/arena.c
