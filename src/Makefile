CC=gcc
CFLAGS=-Os -s -flto -ffunction-sections -fdata-sections --static
DEPS=request.h cloudflare.h
OBJ=main.o requests.o cloudflare.o
STATIC=/usr/lib/libcurl.a /usr/lib/libnghttp2.a /usr/lib/libidn2.a /usr/lib/libunistring.a /usr/lib/libbrotlidec.a /usr/lib/libbrotlicommon.a /usr/lib/libcrypto.a /usr/lib/libssl.a /lib/libz.a

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $@ $^ $(STATIC) $(CFLAGS)