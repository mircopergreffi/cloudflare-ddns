CC=gcc
CFLAGS=-Os -s -flto -ffunction-sections -fdata-sections --static
DBGCFLAGS=-g3 --static
DEPS=request.h cloudflare.h parameters.h mjson.h
OBJ=main.o requests.o cloudflare.o parameters.o mjson.o
STATIC=/usr/lib/libcurl.a /usr/lib/libnghttp2.a /usr/lib/libidn2.a /usr/lib/libunistring.a /usr/lib/libbrotlidec.a /usr/lib/libbrotlicommon.a /usr/lib/libcrypto.a /usr/lib/libssl.a /lib/libz.a

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $@ $^ $(STATIC) $(CFLAGS)

debug: $(OBJ)
	$(CC) -o $@ $^ $(STATIC) $(DBGCFLAGS)

clean:
	rm -f *.o build debug