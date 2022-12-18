FROM alpine AS build
WORKDIR /root/build
RUN apk add --no-cache build-base git libcurl
RUN git clone https://github.com/cesanta/mjson
COPY src ./src
CMD sh

# FROM busybox:1.35.0-musl
# COPY --from=build /root/build/cfddns /bin/cfddns
# ENTRYPOINT ["/bin/cfddns"]