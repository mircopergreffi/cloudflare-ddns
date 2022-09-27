# Cloudflare DNS Updater

Checks for IP changes and updates your Cloudflare DNS records.  
It allows you to use Cloudflare as a Dynamic DNS.  
Uses Cloudflare python library.  
Configuration in YAML.

## Prerequisites
This documentation assumes you already have a Cloudflare account set up.

You have generated an API token (see https://dash.cloudflare.com/profile/api-tokens).  
You know the Record ID and Zone ID of the records you want to update (see https://api.cloudflare.com) 

## Install by Cloning

It requires Python3 and pip for dependencies.

Check python version:
```
$ python --version
Python 3.10.5
```

Clone the repository
```
$ git clone https://github.com/mircopergreffi/cloudflare-ddns-updater
$ cd cloudflare-ddns-updater
```
Copy the configuration template (`example-config.yaml`) as `config.yaml` and fill it:
```
$ cp example-config.yaml config.yaml
$ nano config.yaml
```

Install dependencies and run:
```
$ python -m pip install -r requirements.txt
$ python main.py
```

## Install with Docker
Otherwise you can use Docker.  
This is usually the preferred choice.  
As of now it is limited to just one record using environment variables, but you can update the `config.yaml` file after creating and running the container to update more than one record.

### Prebuilt image
You can use the built image with the command:  
```
docker run -d \
  -e CLOUDFLARE_EMAIL="replace-with-your-email" \
  -e CLOUDFLARE_TOKEN="replace-with-your-token" \
  -e CLOUDFLARE_RECORD_ID="replace-with-your-record-id" \
  -e CLOUDFLARE_ZONE_ID="replace-with-your-zone-id" \
  mircopergreffi/cloudflare_ddns_updater:latest
```

### Dockerfile
Otherwise, if you prefer, you can build the image yourself using the Dockerfile.