#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * get_env(const char *name)
{
    char *value = getenv(name);
    if (value == NULL)
    {
        printf("Environment variable %s not set\n", name);
        exit(1);
    }
    return value;
}

char * get_env_and_print(const char *name)
{
    char *value = get_env(name);
    printf("%s: %s\n", name, value);
    return value;
}

char* str_replace(char *str, const char *pattern, const char *newvalue)
{
    size_t pattern_len = strlen(pattern);
    size_t newvalue_len = strlen(newvalue);
    size_t str_len = strlen(str);
    size_t count = 0;
    char *ptr = str;
    
    while ((ptr = strstr(ptr, pattern)) != NULL)
    {
        count++;
        ptr += pattern_len;
    }

    size_t new_str_len = str_len + count * (newvalue_len - pattern_len);
    char *new_str = (char*) malloc(new_str_len + 1);
    char *to_ret = new_str;
    
    char *ptr2 = str;
    ptr = str;
    size_t remaining = str_len - (ptr2 - str);

    while ((ptr = strstr(ptr2, pattern)) != NULL)
    {
        size_t len = ptr - ptr2;
        memcpy(new_str, ptr2, len);
        memcpy(new_str + len, newvalue, newvalue_len);

        new_str += len + newvalue_len;
        ptr2 = ptr + pattern_len;
        remaining = str_len - (ptr2 - str);
    }
    memcpy(new_str, ptr2, remaining + 1);

    return to_ret;
}

char * replace_bind(char *bind_template, const char *ip, const char *domain)
{
    char *a = str_replace(bind_template, "%ip%", ip);
    char *b = str_replace(a, "%domain%", domain);
    char *c = str_replace(b, ";", "\n");
    free(a);
    free(b);
    return c;
}


Parameters get_parameters()
{
    Parameters parameters;
    parameters.token = get_env("TOKEN");
    parameters.zone_id = get_env_and_print("ZONEID");
    parameters.update_interval = atoi(get_env_and_print("UPDATE_INTERVAL"));
    parameters.domain = get_env_and_print("DOMAIN");
    parameters.bind_template = get_env_and_print("BIND_TEMPLATE");
    parameters.bind_template_noproxy = get_env_and_print("BIND_TEMPLATE_NOPROXY");
    parameters.skip_cname = atoi(get_env_and_print("SKIP_CNAME"));
    return parameters;
}