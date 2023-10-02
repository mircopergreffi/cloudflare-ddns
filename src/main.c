
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cloudflare.h"
#include "parameters.h"
#include "mjson.h"

#define IP_HAYSTACK "\nip="

void get_ip(char *ip)
{
    Response res = request("GET", "https://cloudflare.com/cdn-cgi/trace", NULL, 0);
    size_t index = 0;
    size_t ip_index = 0;
    size_t haystack_len = strlen(IP_HAYSTACK);
    while (index < res.size - haystack_len)
    {
        if (strncmp(IP_HAYSTACK, res.memory + index, haystack_len) == 0)
        {
            index += haystack_len;
            while (res.memory[index] != '\n')
            {
                ip[ip_index++] = res.memory[index++];
            }
            ip[ip_index] = '\0';
            break;
        }
        index++;
    }
    response_cleanup(res);
}

void handle_zones(const Parameters params, const CloudFlare cloudflare, const Response response)
{
    const char *buf;
    int len;
    if (mjson_find(response.memory, response.size, "$.result", &buf, &len) != MJSON_TOK_ARRAY)
    {
        printf("Error while parsing json: $.result is not an array\n");
        exit(1);
    }
    // printf(">>%.*s<<\n", len, buf);

    int next = 0;
    int koff = 0;
    int klen = 0;
    char selector[128];

    const char *itemBuf;
    int itemLen = 0;

    const char *typeBuf;
    int typeLen = 0;
    const char *idBuf;
    int idLen = 0;
    const char *nameBuf;
    int nameLen = 0;

    char url[256];

    while (1)
    {
        next = mjson_next(buf, len, next, &koff, &klen, NULL, NULL, NULL);
        if (next == 0)
        {
            break;
        }

        sprintf(selector, "$[%d]", koff);
        if (mjson_find(buf, len, selector, &itemBuf, &itemLen) != MJSON_TOK_OBJECT)
        {
            printf("Error while parsing json: %s is not an object\n", selector);
            exit(1);
        }

        if (mjson_find(itemBuf, itemLen, "$.type", &typeBuf, &typeLen) != MJSON_TOK_STRING)
        {
            printf("Error while parsing json: $.type is not a string\n");
            exit(1);
        }

        if (mjson_find(itemBuf, itemLen, "$.id", &idBuf, &idLen) != MJSON_TOK_STRING)
        {
            printf("Error while parsing json: $.id is not a string\n");
            exit(1);
        }

        if (mjson_find(itemBuf, itemLen, "$.name", &nameBuf, &nameLen) != MJSON_TOK_STRING)
        {
            printf("Error while parsing json: $.name is not a string\n");
            exit(1);
        }

        if (strcmp(typeBuf, "CNAME") == 0 && params.skip_cname)
        {
            printf("Skipping CNAME id=%.*s name=%.*s\n", idLen, idBuf, nameLen, nameBuf);
        }
        else
        {
            printf("Deleting record id=%.*s name=%.*s\n", idLen, idBuf, nameLen, nameBuf);
            sprintf(url, "https://api.cloudflare.com/client/v4/zones/%s/dns_records/%.*s", params.zone_id, idLen, idBuf);
            Response mem = cloudflare_request(cloudflare, "DELETE", url);
            printf("%s\n", mem.memory);
            response_cleanup(mem);
        }
    }

    free((void *)buf);
    free((void *)itemBuf);
    free((void *)typeBuf);
    free((void *)idBuf);
    free((void *)nameBuf);
}


int main(int argc, char** argv)
{
    printf("Reading environment variables:\n");
    Parameters params = get_parameters();

    char ip[32];
    get_ip(ip);
    printf("IP: %s\n", ip);

    printf("Initializing CloudFlare API:\n");
    CloudFlare cloudflare = cloudflare_init(params.token);

    printf("Requesting zones:\n");
    char url[256];
    sprintf(url, "https://api.cloudflare.com/client/v4/zones/%s/dns_records", params.zone_id);
    Response mem = cloudflare_request(cloudflare, "GET", url);
    // printf(">>%s<<\n", mem.memory);
    handle_zones(params, cloudflare, mem);
    response_cleanup(mem);

    printf("Cleaning up CloudFlare:\n");
    cloudflare_cleanup(cloudflare);
    // Cleanup global curl

    printf("Cleaning up curl:\n");
    curl_global_cleanup();

    printf("Exiting\n");
    return 0;
}