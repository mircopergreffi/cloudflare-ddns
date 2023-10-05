
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cloudflare.h"
#include "parameters.h"
#include "mjson.h"
#include "log_utils.h"
#include "sub.h"
#include "errors.h"
#include <unistd.h>

#define TRY(x) if ((x) != NO_ERROR) { continue; }

int main(int argc, char** argv)
{
    LOG_INFO("Reading environment variables");
    struct Parameters params = get_parameters();

    char ip[32];
    char previousIp[32];

    char first = 1;
    do
    {
        if (params.update_interval > 0 && first == 0)
        {
            LOG_INFO("Sleeping for %d seconds", params.update_interval);
            sleep(params.update_interval);
        }
        first = 0;

        TRY(sub_get_ip(ip));

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
            TRY(sub_handle_zones(params, cloudflare, mem));
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
    } while (params.update_interval > 0);
    // Cleanup global curl
    LOG_INFO("Cleaning up curl");
    curl_global_cleanup();

    LOG_INFO("Exiting");
    return 0;
}