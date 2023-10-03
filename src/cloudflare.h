
#ifndef CLOUDFLARE_H
#define CLOUDFLARE_H

#include "requests.h"

#define CLOUDFLARE_PROXIED "true"
#define CLOUDFLARE_NOT_PROXIED "false"

struct CloudFlare_blob
{
    char *token;
    char *authorization_header;
};

typedef struct CloudFlare_blob CloudFlare;

// Init Cloudflare API
extern CloudFlare cloudflare_init(char *token);

// Clear Cloudflare API
extern void cloudflare_cleanup(const CloudFlare cloudflare);

// Cloudflare API request
extern Response cloudflare_request(const CloudFlare cloudflare, const char *method, const char *url);

extern void cloudflare_import(const CloudFlare cloudflare, const char* zone_id, const char* bind, const char* proxied);

#endif /* CLOUDFLARE_H */