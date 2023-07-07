
#include "requests.h"
#include <curl/curl.h>

struct curl_slist *headers = NULL;

CURL * request_init()
{
    // Init curl
    CURL *curl = curl_easy_init();
    return curl;
}

void request_cleanup(CURL *curl)
{
    // Cleanup curl
    if (curl) curl_easy_cleanup(curl);
    // Cleanup global curl
    curl_global_cleanup();
}

void request_prepare(CURL *curl, char *method, char *URL)
{
    // Log
    printf("Request: %s %s", method, URL);
	// Set url
	curl_easy_setopt(curl, CURLOPT_URL, URL);
	// Set method
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    // Disable console output
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
}

void request_set_header(CURL *curl, char *header)
{
	// Set header
	headers = curl_slist_append(headers, header);
}

CURLcode request_perform(CURL *curl)
{
	// Set headers if any
	if (headers != NULL)
	{
		// Enable headers
		curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		// Set headers list
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}
	// Perform the request, res will get the return code
	CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        printf("Response code: %ld", response_code);
    }
    else
    {
        printf("Request failed: %s", curl_easy_strerror(res));
    }
	return res;
}
