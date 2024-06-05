#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log_utils.h"

char * get_env(const char *name)
{
    TRACE_START();
    LOG_DEBUG("Getting environment variable %s", name);
    char *value = getenv(name);
    if (value == NULL)
    {
        LOG_ERROR("Environment variable %s not set", name);
        exit(1);
    }
    LOG_DEBUG("Environment variable %s: %s", name, value);
    TRACE_END();
    return value;
}

char * get_env_and_print(const char *name)
{
    TRACE_START();
    char *value = get_env(name);
    LOG_INFO("%s: %s", name, value);
    TRACE_END();
    return value;
}

char* str_replace(char *str, const char *pattern, const char *newvalue)
{
    TRACE_START();
    LOG_DEBUG("Replacing %s with %s in %s", pattern, newvalue, str);

    size_t pattern_len = strlen(pattern);
    size_t newvalue_len = strlen(newvalue);
    size_t str_len = strlen(str);
    size_t count = 0;

    LOG_DEBUG("Pattern length: %zu", pattern_len);
    LOG_DEBUG("New value length: %zu", newvalue_len);
    LOG_DEBUG("String length: %zu", str_len);

    char *ptr = str;
    
    while ((ptr = strstr(ptr, pattern)) != NULL)
    {
        count++;
        ptr += pattern_len;
    }

    LOG_DEBUG("Found %zu occurences", count);

    size_t new_str_len = str_len + count * (newvalue_len - pattern_len);

    LOG_DEBUG("New string length: %zu", new_str_len);

    LOG_DEBUG("Allocating %zu bytes", new_str_len + 1)
    char *new_str = (char*) malloc(new_str_len + 1);
    char *to_ret = new_str;
    
    char *ptr2 = str;
    ptr = str;
    size_t remaining = str_len - (ptr2 - str);

    while ((ptr = strstr(ptr2, pattern)) != NULL)
    {
        size_t len = ptr - ptr2;
        LOG_DEBUG("Copying substring, %zu bytes", len);
        memcpy(new_str, ptr2, len);
        LOG_DEBUG("Copying newvalue, %zu bytes", newvalue_len);
        memcpy(new_str + len, newvalue, newvalue_len);
        new_str += len + newvalue_len;
        ptr2 = ptr + pattern_len;
        remaining = str_len - (ptr2 - str);
        LOG_DEBUG("Remaining: %zu", remaining);
    }
    LOG_DEBUG("Copying remaining, %zu +1 bytes", remaining);
    memcpy(new_str, ptr2, remaining + 1);

    TRACE_END();
    return to_ret;
}

char * replace_bind(char *bind_template, const char *ip, const char *domain)
{
    TRACE_START();
    char *a = str_replace(bind_template, "%ip%", ip);
    char *b = str_replace(a, "%domain%", domain);
    char *c = str_replace(b, ";", "\n");
    LOG_DEBUG("Freeing a and b");
    free(a);
    free(b);
    TRACE_END();
    return c;
}

struct Parameters get_parameters()
{
    TRACE_START();
    struct Parameters parameters;
    parameters.token = get_env("TOKEN");
    parameters.zone_id = get_env_and_print("ZONEID");
    parameters.update_interval = atoi(get_env_and_print("UPDATE_INTERVAL"));
    parameters.domain = get_env_and_print("DOMAIN");
    parameters.bind_template = get_env_and_print("BIND_TEMPLATE");
    parameters.bind_template_noproxy = get_env_and_print("BIND_TEMPLATE_NOPROXY");
    parameters.skip_cname = atoi(get_env_and_print("SKIP_CNAME"));
    parameters.healthcheck_url = get_env_and_print("HEALTHCHECK_URL");
    TRACE_END();
    return parameters;
}