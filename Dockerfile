FROM alpine AS build
WORKDIR /root/build
COPY src ./src
RUN apk add --no-cache build-base git curl-dev
RUN git clone https://github.com/cesanta/mjson
RUN gcc -o cfddns src/main.c -lcurl
CMD ./cfddns

# FROM busybox:1.35.0-musl
# WORKDIR /app
# COPY --from=build /root/build/cfddns /app/cfddns
# ENTRYPOINT ["./cfddns"]