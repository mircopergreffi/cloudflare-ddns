# Cloudflare DDNS

Simple C application wrapped into a container for updating Cloudflare DNS records.  
It allows you to use Cloudflare as a Dynamic DNS.  
  
The container is built from scratch and only contains the executable and SSL certficates in order to minimize its footprint.
  
The uncompressed image size is **5.6MB**.  

RAM usage is roughly **1MB**.
  
A special thanks to [@gmasini97](https://github.com/gmasini97) for the contributions.

## How It Works

Waits for your public IP to change, it checks for changes with a set time interval (`UPDATE_INTERVAL`).  
The IP is checked using Cloudflare APIs (https://cloudflare.com/cdn-cgi/trace).  
Then deletes all DNS records (except for CNAME records).  
Generate new DNS records from the specified template (`BIND_TEMPLATE` and `BIND_TEMPLATE_NOPROXY`).  
Set `UPDATE_INTERVAL` to a negative number (i.e.: `-1`) for updating the DNS records once and exiting.  

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
Replace `YOUR_TOKEN` with the previously generated token.  

## Environment Variables

| Variable              | Description | Example |
|-----------------------|-------------|---------|
| TOKEN                 | Cloudflare API Token | |
| ZONEID                | Cloudflare Zone ID | |
| UPDATE_INTERVAL       | Time to wait between IP checks in seconds. `-1` for updating DNS records once. | 30 |
| SKIP_CNAME            | Wheter to skip CNAME records. Set to `1` for skipping, `0` for deleting them. | `1` |
| DOMAIN                | Your domain name | `example.com` |
| BIND_TEMPLATE         |  | `%domain%. 1 IN A %ip%` |
| BIND_TEMPLATE_NOPROXY |  |  |

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
$ git clone https://github.com/mircopergreffi/cloudflare-ddns
$ cd cloudflare-ddns
```

Create `.env` file from `example.env` and edit it:
```
$ cp example.env .env
$ vi .env
```

Build the image:
```
$ docker build -t cloudflare-ddns .
```

Run the image:
```
$ docker run -d --env-file .env cloudflare-ddns
```

### Prebuilt image

Create a `.env` file from the template (`example.env`).  
Pull and run the image:
```
$ docker run -d --env-file .env \
    mircopergreffi/cloudflare-ddns

```
