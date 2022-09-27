# Cloudflare DNS Updater

Checks for IP changes and updates your Cloudflare DNS records.  
It allows you to use Cloudflare as a Dynamic DNS.  
Uses Cloudflare python library.  

## Install by Cloning
Clone the repository  
    `$ git clone https://github.com/mircopergreffi/cloudflare-ddns-updater`  
    `$ cd cloudflare-ddns-updater`  

## Install with Docker  
Otherwise you can use Docker.  

Run the container with the command:  
    `docker run -d \`
    `  -e CLOUDFLARE_EMAIL="example@example.com" \`  
    `  -e CLOUDFLARE_TOKEN="" \`  
    `  -e CLOUDFLARE_RECORD_ID="" \`  
    `  -e CLOUDFLARE_ZONE_ID="" \`  
    `  mircopergreffi/cloudflare_ddns_updater:latest`
