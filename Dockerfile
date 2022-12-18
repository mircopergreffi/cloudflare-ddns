FROM alpine AS build


FROM busybox:1.35.0-uclibc
COPY --from=build cfddns /bin/cfddns
ENTRYPOINT ["/bin/cfddns"]