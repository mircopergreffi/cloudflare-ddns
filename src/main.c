
#include <stdio.h>
#include <stdlib.h>
// #include <curl/curl.h>
#include "cloudflare.h"

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

int main(int argc, char** argv)
{
	printf("Reading environment variables:\n");
	char *token = get_env("TOKEN");
	char *zoneid = get_env_and_print("ZONEID");
	char *update_interval = get_env_and_print("UPDATE_INTERVAL");
	char *domain = get_env_and_print("DOMAIN");
	char *bind_template = get_env_and_print("BIND_TEMPLATE");
	char *bind_template_noproxy = get_env_and_print("BIND_TEMPLATE_NOPROXY");
	printf("--------------------------------\n");
	printf("Initializing Cloudflare API:\n");
	CloudFlare *cloudflare = cloudflare_init(token);
	if (cloudflare)
	{
		printf("Cloudflare API initialized\n");
		printf("Requesting zones:\n");
		cloudflare_request(cloudflare, "GET", "https://api.cloudflare.com/client/v4/zones");
	}
	else
	{
		printf("Cloudflare API initialization failed\n");
	}
	cloudflare_cleanup(cloudflare);
	return 0;
}