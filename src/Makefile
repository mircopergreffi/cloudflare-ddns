CC=gcc

ifeq ($(DEBUG), 1)
	ifndef LOG_LEVEL
		LOG_LEVEL=0
	endif
	CFLAGS=-g3 --static -D LOG_LEVEL=$(LOG_LEVEL)
else
	ifndef LOG_LEVEL
		LOG_LEVEL=1000
	endif
	CFLAGS=-Os -s -flto -ffunction-sections -fdata-sections --static -D LOG_LEVEL=$(LOG_LEVEL)
endif

DEPS=request.h cloudflare.h parameters.h mjson.h reponse.h request_result.h log_utils.h

OBJ=main.o requests.o cloudflare.o parameters.o mjson.o request_result.o response.o

STATIC=/usr/lib/libcurl.a /usr/lib/libnghttp2.a /usr/lib/libidn2.a /usr/lib/libunistring.a /usr/lib/libbrotlidec.a /usr/lib/libbrotlicommon.a /usr/lib/libcrypto.a /usr/lib/libssl.a /lib/libz.a

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

build: $(OBJ)
	$(CC) -o $@ $^ $(STATIC) $(CFLAGS)

clean:
	rm -f *.o build debug