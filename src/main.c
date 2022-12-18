
#include <stdio.h>
#include <curl/curl.h>


void get_ip(CURL *handle, char* ip)
{
	curl_easy_setopt(handle, CURLOPT_URL, "https://cloudflare.com/cdn-cgi/trace");
	CURLcode res = curl_easy_perform(handle);
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

	char key[20], value[20];
	sscanf(chunk.response, "%s=%s", &key, &value);
	printf(key);
}

void get_dns_records(CURL *handle)
{

}

int main(int argc, char** argv)
{
	// system("export IP=$(wget -qO - https://cloudflare.com/cdn-cgi/trace | grep ip | sed \"s/ip=//g\")");
	// system("echo $IP");

	// printf("%s",getenv("IP"));

	// printf("ZONEID = %s\n",getenv("ZONEID"));
	// printf("UPDATE_INTERVAL = %s\n",getenv("UPDATE_INTERVAL"));
	// printf("DOMAIN = %s\n",getenv("DOMAIN"));
	// printf("BIND_TEMPLATE = %s\n",getenv("BIND_TEMPLATE"));
	// printf("BIND_TEMPLATE_NOPROXY = %s\n",getenv("BIND_TEMPLATE_NOPROXY"));


	// Initialize libcurl
	CURL *curl;
	curl = curl_easy_init();
	if (!curl)
	{
		printf("Cannot initialize libcurl");
	}
	else
	{
		// Print libcurl version
		printf("libcurl version %s\n", curl_version());
		// Curl cleanup
		curl_easy_cleanup(curl);
	}

	// Global cleanup
	curl_global_cleanup();

	return 0;
}