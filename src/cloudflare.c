
#include <string.h>
#include <stdlib.h>
#include "cloudflare.h"
#include "requests.h"

#define HEADER_AUTH "Authorization: Bearer "

CloudFlare cloudflare_init(char *token)
{
    // Allocate memory for cloudflare
    CloudFlare cloudflare = {};
    // Set curl and token
    cloudflare.token = token;
    cloudflare.authorization_header = (char*) malloc(strlen(HEADER_AUTH) + strlen(token) + 1);
    strcat(cloudflare.authorization_header, HEADER_AUTH);
    strcat(cloudflare.authorization_header, token);
    // Return cloudflare
    return cloudflare;
}

void cloudflare_cleanup(const CloudFlare cloudflare)
{
    // Cleanup token
    free(cloudflare.authorization_header);
}

// Cloudflare API request
Response cloudflare_request(const CloudFlare cloudflare, const char *method, const char *url)
{
    const char *headers[] = {cloudflare.authorization_header, "Content-Type: application/json"};
    return request(method, url, headers, 2);
}