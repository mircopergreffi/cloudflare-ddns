
#include <string.h>
#include <stdlib.h>
#include "cloudflare.h"
#include "requests.h"
#include "log_utils.h"

#define HEADER_AUTH "Authorization: Bearer "
#define API_THROTTLE_SLEEP 1

CloudFlare cloudflare_init(char *token)
{
    // Allocate memory for cloudflare
    CloudFlare cloudflare = {};
    // Set curl and token
    cloudflare.token = token;
    size_t auth_len = strlen(HEADER_AUTH) + strlen(token) + 1;

    cloudflare.authorization_header = (char*) malloc(auth_len);
    snprintf(cloudflare.authorization_header, auth_len, "%s%s", HEADER_AUTH, token);
    // Return cloudflare
    return cloudflare;
}

void cloudflare_cleanup(const CloudFlare cloudflare)
{
    // Cleanup token
    free(cloudflare.authorization_header);
}

// Cloudflare API request
struct RequestResult cloudflare_request(const CloudFlare cloudflare, const char *method, const char *url)
{
    sleep(API_THROTTLE_SLEEP);
    if (strncmp(url, "https", 5) != 0){
        LOG_ERROR("Error: url must be https");
        exit(1);
    } else {
        const char *headers[] = {cloudflare.authorization_header, "Content-Type: application/json"};
        return request(method, url, headers, 2);
    }
}

void cloudflare_import(const CloudFlare cloudflare, const char* zone_id, const char* bind, const char* proxied)
{
    sleep(API_THROTTLE_SLEEP);
    char url[256];
    snprintf(url, 256, "https://api.cloudflare.com/client/v4/zones/%s/dns_records/import", zone_id);

    const char *headers[] = {cloudflare.authorization_header, "Content-Type: multipart/form-data"};
    struct RequestResult r = request_multipart(
        "POST",
        url,
        headers,
        2,
        (struct MultipartData[]){
            {"file", bind},
            {"proxied", proxied}
        },
        2
    );

    request_result_cleanup(&r);
}
