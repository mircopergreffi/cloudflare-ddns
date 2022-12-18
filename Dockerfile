FROM alpine
RUN apk add --no-cache curl jq
COPY run.sh /app/run.sh
ENTRYPOINT ["sh","/app/run.sh"]