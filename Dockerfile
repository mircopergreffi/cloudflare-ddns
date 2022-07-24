FROM python:alpine

RUN apk add git
RUN git clone https://github.com/mircopergreffi/cloudflare-ddns-updater
# RUN python -m pip install -r requirements.txt

CMD sh #python cloudflare-ddns-updater/cloudflare-ddns-updater.py