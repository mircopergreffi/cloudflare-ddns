
#include <string.h>
#include "cloudflare.h"
#include "requests.h"

#define HEADER_AUTH "Authorization: Bearer "

CloudFlare * cloudflare_init(char *token)
{
	CURL * curl = request_init();
	if (!curl) return NULL;
	// Allocate memory for cloudflare
	CloudFlare *cloudflare = (CloudFlare*) malloc(sizeof(CloudFlare));
	if (!cloudflare) return NULL;
	// Set curl and token
	cloudflare->curl = curl;
	cloudflare->token = (char*) malloc(strlen(HEADER_AUTH) + strlen(token) + 1);
	strcat(cloudflare->token, HEADER_AUTH);
	strcat(cloudflare->token, token);
	printf("Token: %s\n", cloudflare->token);
	// Return cloudflare
	return cloudflare;
}

void cloudflare_cleanup(CloudFlare *cloudflare)
{
	// Cleanup curl
	request_cleanup(cloudflare->curl);
	// Cleanup token
	free(cloudflare->token);
	// Cleanup cloudflare
	free(cloudflare);
}

// Cloudflare API request
void cloudflare_request(CloudFlare *cloudflare, char *method, char *url)
{
	// Prepare request
	request_prepare(cloudflare->curl, method, url);
	// Set headers
	request_set_header(cloudflare->curl, "User-Agent: libcurl-agent/1.0");
	request_set_header(cloudflare->curl, cloudflare->token);
	request_set_header(cloudflare->curl, "Content-Type: application/json");
	// Perform request
	return request_perform(cloudflare->curl);
}