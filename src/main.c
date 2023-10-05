
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cloudflare.h"
#include "parameters.h"
#include "mjson.h"
#include "log_utils.h"

#define IP_HAYSTACK "\nip="

void get_ip(char *ip)
{
    struct RequestResult result = request("GET", "https://cloudflare.com/cdn-cgi/trace", NULL, 0);
    struct Response res = result.response;
    size_t index = 0;
    size_t ip_index = 0;
    size_t haystack_len = strlen(IP_HAYSTACK);
    while (index < res.content_size - haystack_len)
    {
        if (strncmp(IP_HAYSTACK, res.content + index, haystack_len) == 0)
        {
            index += haystack_len;
            while (res.content[index] != '\n')
            {
                ip[ip_index++] = res.content[index++];
            }
            ip[ip_index] = '\0';
            break;
        }
        index++;
    }
    request_result_cleanup(&result);
}

void handle_zones(const struct Parameters params, const CloudFlare cloudflare, const struct Response response)
{
    const char *buf;
    int len;
    if (mjson_find(response.content, response.content_size, "$.result", &buf, &len) != MJSON_TOK_ARRAY)
    {
        LOG_ERROR("Error while parsing json: $.result is not an array");
        exit(1);
    }

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
            LOG_ERROR("Error while parsing json: %s is not an object", selector);
            exit(1);
        }

        if (mjson_get_string(itemBuf, itemLen, "$.type", typeBuf, 256) < 0)
        {
            LOG_ERROR("Error while parsing json: $.type is not a string");
            exit(1);
        }

        if (mjson_get_string(itemBuf, itemLen, "$.id", idBuf, 256) < 0)
        {
            LOG_ERROR("Error while parsing json: $.id is not a string");
            exit(1);
        }

        if (mjson_get_string(itemBuf, itemLen, "$.name", nameBuf, 256) < 0)
        {
            LOG_ERROR("Error while parsing json: $.name is not a string");
            exit(1);
        }

        if (strcmp(typeBuf, "CNAME") == 0 && params.skip_cname)
        {
            LOG_INFO("Skipping CNAME id=%s name=%s", idBuf, nameBuf);
        }
        else
        {
            LOG_INFO("Deleting record id=%s name=%s", idBuf, nameBuf);
            snprintf(url, 256, "https://api.cloudflare.com/client/v4/zones/%s/dns_records/%s", params.zone_id, idBuf);
            struct RequestResult rr = cloudflare_request(cloudflare, "DELETE", url);
            struct Response mem = rr.response;
            request_result_cleanup(&rr);
        }
    }
}


int main(int argc, char** argv)
{
    LOG_INFO("Reading environment variables");
    struct Parameters params = get_parameters();

    char ip[32];
    char previousIp[32];

    do
    {
        get_ip(ip);
        LOG_INFO("last IP: %s", previousIp);
        LOG_INFO("current IP: %s", ip);

        char *bind = replace_bind(params.bind_template, ip, params.domain);
        char *bind_noproxy = replace_bind(params.bind_template_noproxy, ip, params.domain);
        LOG_INFO("---- BIND:\n%s\n----", bind);
        LOG_INFO("---- BIND_NOPROXY:\n%s\n----", bind_noproxy);

        if (strcmp(ip, previousIp) != 0)
        {
            LOG_INFO("Initializing CloudFlare API");
            CloudFlare cloudflare = cloudflare_init(params.token);

            LOG_INFO("Requesting zones");
            char url[256];
            snprintf(url, 256, "https://api.cloudflare.com/client/v4/zones/%s/dns_records", params.zone_id);
            struct RequestResult rr = cloudflare_request(cloudflare, "GET", url);
            struct Response mem = rr.response;
            // printf(">>%s<<\n", mem.memory);
            handle_zones(params, cloudflare, mem);
            request_result_cleanup(&rr);

            LOG_INFO("Importing BIND");
            cloudflare_import(cloudflare, params.zone_id, bind, CLOUDFLARE_PROXIED);

            LOG_INFO("Importing BIND_NOPROXY");
            cloudflare_import(cloudflare, params.zone_id, bind_noproxy, CLOUDFLARE_NOT_PROXIED);

            LOG_INFO("Cleaning up CloudFlare");
            cloudflare_cleanup(cloudflare);

            LOG_INFO("Updating previous IP");
            strcpy(previousIp, ip);
        }

        if (params.update_interval > 0)
        {
            LOG_INFO("Sleeping for %d seconds", params.update_interval);
            sleep(params.update_interval);
        }
    } while (params.update_interval > 0);
    // Cleanup global curl
    LOG_INFO("Cleaning up curl");
    curl_global_cleanup();

    LOG_INFO("Exiting");
    return 0;
}