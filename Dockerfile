FROM python:alpine

RUN apk add git
RUN git clone https://github.com/mircopergreffi/cloudflare-ddns-updater
#RUN python -m pip install -r /cloudflare-ddns-updater/requirements.txt

CMD sh #python /cloudflare-ddns-updater/main.py