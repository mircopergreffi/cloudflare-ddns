FROM alpine AS build
WORKDIR /root/build
RUN apk add --no-cache build-base git yaml-dev curl-dev curl-static nghttp2-static libidn2-static libunistring-static brotli-static openssl-libs-static libgcrypt-static zlib-static
COPY src .
RUN make build
# CMD ldd build 

FROM busybox:1.36.1-musl
# FROM alpine
COPY --from=build /root/build/build /app/cfddns
# ENTRYPOINT [ "tail", "-f", "/dev/null" ]
ENTRYPOINT ["/app/cfddns"]