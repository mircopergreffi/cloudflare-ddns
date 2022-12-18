# Cloudflare DDNS

Simple bash script for updating Cloudflare DNS records.  
It allows you to use Cloudflare as a Dynamic DNS.  
  
The uncompressed image size is almost 10MB.  

## How It Works

Checks for IP changes and updates your Cloudflare DNS records.  
CNAME records are not deleted.  
...

## Prerequisites
This documentation assumes you already have a Cloudflare account set up with a domain registered.  

### Generate an API Token

Visit https://dash.cloudflare.com/profile/api-tokens.  
Click the "Create Token" button.  
Use the template "Edit zone DNS".  
In the "Zone Resources" section choose: "Include", "Specific Zone" and the zone.  
Scroll to the bottom and click the "Continue to summary" button.  
Click the "Create Token" button.  
Copy your newly generate token.  

### List your ZoneIDs

Open a terminal and type:  
```
curl -X GET "https://api.cloudflare.com/client/v4/zones" \
     -H "Authorization: Bearer YOUR_TOKEN" \
     -H "Content-Type: application/json"
```
Replace YOUR_TOKEN with the previously generated token.  

## Environment Variables

| Variable              | Description | Example |
|-----------------------|-------------|---------|
| TOKEN                 | Cloudflare API Token | |
| ZONEID                | Cloudflare Zone ID | |
| UPDATE_INTERVAL       | Time to wait between IP checks in seconds. | 30 |
| DOMAIN                | Your domain name | example.com |
| BIND_TEMPLATE         |  | %domain%. 1 IN A %ip% |
| BIND_TEMPLATE_NOPROXY |  | %domain% 1 IN TXT "v=spf1 ip4:%ip% -all" |

`BIND_TEMPLATE` and `BIND_TEMPLATE_NOPROXY` are used to generate `Zone files` (see: https://en.wikipedia.org/wiki/Zone_file).  
The DNS records generated from `BIND_TEMPLATE` are proxied from Cloudflare.  
The DNS records generated from `BIND_TEMPLATE_NOPROXY` are **NOT** proxied from Cloudflare.  
Template variables:  
 - `%domain%` is replaced with the content of the environment variable `DOMAIN`.  
 - `%ip%` is replaced with your public IP.  
 - `;` is replaced with a new line.  

## Docker

### Build the image yourself

Pull the repository:
```
$ git clone -b alpine-sh https://github.com/mircopergreffi/cloudflare-ddns
$ cd cloudflare-ddns
```

Create `.env` file from `example.env` and edit it:
```
$ cp example.env .env
$ vi .env
```

Build the image:
```
$ docker build -t cloudflare_ddns:alpine-sh .
```

Run the image:
```
$ docker run -d --env-file .env cloudflare_ddns:alpine-sh
```

### Prebuilt image

Create a `.env` file from the template (`example.env`).  
Pull and run the image:
```
$ docker run -d --env-file .env \
    mircopergreffi/cloudflare_ddns:alpine-sh

```
