# Cloudflare DNS Updater

Simple bash script for updating Cloudflare DNS records.  
It allows you to use Cloudflare as a Dynamic DNS.  
  
The image size is less than 13MB.  

## How It Works

Checks for IP changes and updates your Cloudflare DNS records.  
CNAME records are not deleted.  
...

## Prerequisites
This documentation assumes you already have a Cloudflare account set up.  
You have generated an API token (see https://dash.cloudflare.com/profile/api-tokens).  
You know the Zone ID of the records you want to update (see https://api.cloudflare.com) 

## Docker

### Build the image yourself

Pull the repository:
```
$ git clone -b alpine-sh https://github.com/mircopergreffi/cloudflare-ddns-updater
$ cd cloudflare-ddns-updater
```

Create `.env` file from `example.env` and edit it:
```
$ cp example.env .env
$ vi .env
```

Build the image:
```
$ docker build -t cloudflare_ddns_updater:alpine-sh .
```

Run the image:
```
$ docker run -d --env-file .env cloudflare_ddns_updater:alpine-sh
```

### Prebuilt image

Create a `.env` from the template:
```
TOKEN=your_token
ZONEID=your_zone_id
UPDATE_INTERVAL=update_interval_in_seconds__negative_do_not_repeat
DOMAIN=your_domain_name
BIND_TEMPLATE=%domain%. 1 IN A %ip%;*.%domain%. 1 IN CNAME %domain%
BIND_TEMPLATE_NOPROXY=
```

Pull and run the image:
```
$ docker run -d --env-file .env \
    mircopergreffi/cloudflare_ddns_updater:alpine-sh

```