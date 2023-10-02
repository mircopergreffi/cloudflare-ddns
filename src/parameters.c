#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>

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