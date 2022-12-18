FROM alpine
RUN apk add curl jq
COPY run.sh /app/run.sh
CMD /app/run.sh