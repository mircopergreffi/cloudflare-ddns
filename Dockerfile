FROM alpine AS build


FROM busybox:1.35.0-musl
COPY --from=build cfddns /bin/cfddns
ENTRYPOINT ["/bin/cfddns"]