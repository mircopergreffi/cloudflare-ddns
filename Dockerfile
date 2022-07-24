FROM python:alpine

RUN apk add git
RUN git clone https://github.com/mircopergreffi/cloudflare-ddns-updater
RUN python -m pip install -r cloudflare-ddns-updater/requirements.txt

CMD python cloudflare-ddns-updater/cloudflare-ddns-updater.py