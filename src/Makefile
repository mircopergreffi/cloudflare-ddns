CC=gcc
CFLAGS=-I. -lcurl
DEPS=request.h cloudflare.h
OBJ=main.o requests.o cloudflare.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)