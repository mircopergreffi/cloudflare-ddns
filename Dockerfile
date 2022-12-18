FROM alpine AS build
RUN apk add libcurl
COPY src /root/src
CMD sh

# FROM busybox:1.35.0-musl
# COPY --from=build /root/cfddns /bin/cfddns
# ENTRYPOINT ["/bin/cfddns"]