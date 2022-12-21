FROM alpine AS build
WORKDIR /root/build
RUN apk add --no-cache build-base git curl-dev
RUN git clone https://github.com/cesanta/mjson
COPY src .
RUN make build
CMD ./build

# FROM busybox:1.35.0-musl
# COPY --from=build /root/build/build /app/cfddns
# ENTRYPOINT ["/app/cfddns"]