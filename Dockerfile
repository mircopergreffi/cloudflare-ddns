FROM alpine AS build
COPY src /root/src

FROM busybox:1.35.0-musl
COPY --from=build cfddns /bin/cfddns
ENTRYPOINT ["/bin/cfddns"]