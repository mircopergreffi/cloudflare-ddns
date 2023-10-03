
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

    const char typeBuf[256];
    const char idBuf[256];
    const char nameBuf[256];

    char url[256];

    while (1)
    {

        next = mjson_next(buf, len, next, &koff, &klen, NULL, NULL, NULL);
        if (next == 0)
        {
            break;
        }

        snprintf(selector, 128, "$[%d]", koff);
        if (mjson_find(buf, len, selector, &itemBuf, &itemLen) != MJSON_TOK_OBJECT)
        {
            printf("Error while parsing json: %s is not an object\n", selector);
            exit(1);
        }

        if (mjson_get_string(itemBuf, itemLen, "$.type", typeBuf, 256) < 0)
        {
            printf("Error while parsing json: $.type is not a string\n");
            exit(1);
        }

        if (mjson_get_string(itemBuf, itemLen, "$.id", idBuf, 256) < 0)
        {
            printf("Error while parsing json: $.id is not a string\n");
            exit(1);
        }

        if (mjson_get_string(itemBuf, itemLen, "$.name", nameBuf, 256) < 0)
        {
            printf("Error while parsing json: $.name is not a string\n");
            exit(1);
        }

        if (strcmp(typeBuf, "CNAME") == 0 && params.skip_cname)
        {
            printf("Skipping CNAME id=%s name=%s\n", idBuf, nameBuf);
        }
        else
        {
            printf("Deleting record id=%s name=%s\n", idBuf, nameBuf);
            snprintf(url, 256, "https://api.cloudflare.com/client/v4/zones/%s/dns_records/%s", params.zone_id, idBuf);
            Response mem = cloudflare_request(cloudflare, "DELETE", url);
            printf("%s\n", mem.memory);
            response_cleanup(mem);
        }
    }
}


int main(int argc, char** argv)
{
    printf("Reading environment variables:\n");
    Parameters params = get_parameters();

    char ip[32];
    char previousIp[32];

    do
    {
        get_ip(ip);
        printf("last IP: %s\n", previousIp);
        printf("current IP: %s\n", ip);

        char *bind = replace_bind(params.bind_template, ip, params.domain);
        char *bind_noproxy = replace_bind(params.bind_template_noproxy, ip, params.domain);
        printf("---- BIND:\n%s\n----\n", bind);
        printf("---- BIND_NOPROXY:\n%s\n----\n", bind_noproxy);

        if (strcmp(ip, previousIp) != 0)
        {
            printf("Initializing CloudFlare API:\n");
            CloudFlare cloudflare = cloudflare_init(params.token);

            printf("Requesting zones:\n");
            char url[256];
            snprintf(url, 256, "https://api.cloudflare.com/client/v4/zones/%s/dns_records", params.zone_id);
            Response mem = cloudflare_request(cloudflare, "GET", url);
            // printf(">>%s<<\n", mem.memory);
            handle_zones(params, cloudflare, mem);
            response_cleanup(mem);

            printf("Importing BIND:\n");
            cloudflare_import(cloudflare, params.zone_id, bind, CLOUDFLARE_PROXIED);

            printf("Importing BIND_NOPROXY:\n");
            cloudflare_import(cloudflare, params.zone_id, bind_noproxy, CLOUDFLARE_NOT_PROXIED);

            printf("Cleaning up CloudFlare:\n");
            cloudflare_cleanup(cloudflare);

            printf("Updating previous IP:\n");
            strcpy(previousIp, ip);
        }

        if (params.update_interval > 0)
        {
            printf("Sleeping for %d seconds\n", params.update_interval);
            sleep(params.update_interval);
        }
    } while (params.update_interval > 0);
    // Cleanup global curl
    printf("Cleaning up curl:\n");
    curl_global_cleanup();

    printf("Exiting\n");
    return 0;
}