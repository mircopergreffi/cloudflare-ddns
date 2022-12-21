
#ifndef CLOUDFLARE_H
#define CLOUDFLARE_H

#include "requests.h"

struct CloudFlare_blob
{
	char *token;
	CURL *curl;
};

typedef struct CloudFlare_blob CloudFlare;

// Init Cloudflare API
CloudFlare * cloudflare_init(char *token);

// Clear Cloudflare API
void cloudflare_cleanup(CloudFlare *cloudflare);

// Cloudflare API request
void cloudflare_request(CloudFlare *cloudflare, char *method, char *url);

#endif /* CLOUDFLARE_H */