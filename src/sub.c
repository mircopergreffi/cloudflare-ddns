#include "sub.h"
#include "log_utils.h"
#include "errors.h"
#include "mjson.h"

#define IP_HAYSTACK "\nip="

int sub_get_ip(char *ip)
{
    TRACE_START();
    struct RequestResult result = request("GET", "https://cloudflare.com/cdn-cgi/trace", NULL, 0);
    struct Response res = result.response;
    CATCH(result);
    LOG_DEBUG("content address: %p", res.content);

    size_t ip_haystack_len = strlen(IP_HAYSTACK);

    char *haystack = strstr(res.content, IP_HAYSTACK);
    if (haystack == NULL)
    {
        LOG_ERROR("Error while parsing response: %s not found", IP_HAYSTACK);
        return ERROR_GET_IP_HAYSTACK_NOT_FOUND;
    }
    LOG_DEBUG("haystack address: %p", haystack);

    char *end = strstr(haystack + ip_haystack_len, "\n");
    if (end == NULL)
    {
        LOG_ERROR("Error while parsing response: \\n not found");
        return ERROR_GET_IP_NEWLINE_NOT_FOUND;
    }
    LOG_DEBUG("haystack end address: %p", end);

    size_t len = end - haystack - ip_haystack_len;
    LOG_DEBUG("copying %ld bytes", len);
    memcpy(ip, haystack + ip_haystack_len, len);
    ip[len] = '\0';


    // size_t index = 0;
    // size_t ip_index = 0;
    // size_t haystack_len = strlen(IP_HAYSTACK);
    // while (index < res.content_size - haystack_len)
    // {
    //     if (strncmp(IP_HAYSTACK, res.content + index, haystack_len) == 0)
    //     {
    //         index += haystack_len;
    //         while (res.content[index] != '\n')
    //         {
    //             ip[ip_index++] = res.content[index++];
    //         }
    //         ip[ip_index] = '\0';
    //         break;
    //     }
    //     index++;
    // }
    request_result_cleanup(&result);
    TRACE_END();

    return NO_ERROR;
}

int sub_handle_zones(const struct Parameters params, const CloudFlare cloudflare, const struct Response response)
{
    TRACE_START();
    const char *buf;
    int len;

    LOG_DEBUG("finding $.result");
    if (mjson_find(response.content, response.content_size, "$.result", &buf, &len) != MJSON_TOK_ARRAY)
    {
        LOG_ERROR("Error while parsing json: $.result is not an array");
        return ERROR_HANDLE_ZONES_RESULT_NOT_ARRAY;
    }

    int next = 0;
    int koff = 0;
    int klen = 0;
    char selector[128];

    const char *itemBuf;
    int itemLen = 0;

    char typeBuf[256];
    char idBuf[256];
    char nameBuf[256];

    char url[256];

    while (1)
    {
        next = mjson_next(buf, len, next, &koff, &klen, NULL, NULL, NULL);
        LOG_DEBUG("next: %d", koff);
        if (next == 0)
        {
            break;
        }

        snprintf(selector, 128, "$[%d]", koff);
        LOG_DEBUG("selector: %s", selector);
        if (mjson_find(buf, len, selector, &itemBuf, &itemLen) != MJSON_TOK_OBJECT)
        {
            LOG_ERROR("Error while parsing json: %s is not an object", selector);
            return ERROR_HANDLE_ZONES_SELECTOR_NOT_OBJECT;
        }

        if (mjson_get_string(itemBuf, itemLen, "$.type", typeBuf, 256) < 0)
        {
            LOG_ERROR("Error while parsing json: $.type is not a string");
            return ERROR_HANDLE_ZONES_TYPE_NOT_STRING;
        }

        if (mjson_get_string(itemBuf, itemLen, "$.id", idBuf, 256) < 0)
        {
            LOG_ERROR("Error while parsing json: $.id is not a string");
            return ERROR_HANDLE_ZONES_ID_NOT_STRING;
        }

        if (mjson_get_string(itemBuf, itemLen, "$.name", nameBuf, 256) < 0)
        {
            LOG_ERROR("Error while parsing json: $.name is not a string");
            return ERROR_HANDLE_ZONES_NAME_NOT_STRING;
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
            CATCH(rr);
            struct Response mem = rr.response;
            request_result_cleanup(&rr);
        }
    }

    TRACE_END();
    return NO_ERROR;
}