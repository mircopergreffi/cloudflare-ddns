
#include "requests.h"
#include <stdlib.h>
#include <string.h>
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
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    Response *mem = (Response *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void request_prepare(CURL *curl, const char *method, const char *URL, const Response *response)
{
    // Log
    printf("Request: %s %s\n", method, URL);
    // Set url
    curl_easy_setopt(curl, CURLOPT_URL, URL);
    // Set method
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    // Disable console output
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

    if (response != NULL)
    {
        // Set write function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        // Set write data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
    }
}

void request_set_header(CURL *curl, const char *header)
{
    // Set header
    headers = curl_slist_append(headers, header);
}

CURLcode request_perform(CURL *curl)
{
    // Set headers if any
    if (headers != NULL)
    {
        // Disable headers in response
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        // Set headers list
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }
    // Perform the request, res will get the return code
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK)
    {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        printf("Response code: %ld\n", response_code);
    }
    else
    {
        printf("Request failed: %s\n", curl_easy_strerror(res));
    }
    return res;
}

Response request(const char *method, const char *url, const char **headers, const size_t headers_count)
{
    CURL* curl = request_init();
    // Prepare request
    Response response = response_init();
    request_prepare(curl, method, url, &response);
    // Set headers
    request_set_header(curl, "User-Agent: libcurl-agent/1.0");
    for (size_t i = 0; i < headers_count; i++)
    {
        request_set_header(curl, headers[i]);
    }
    // Perform request
    request_perform(curl);
    // Cleanup
    request_cleanup(curl);

    return response;
}

Response response_init()
{
    Response response = {
        .memory = malloc(1),
        .size = 0
    };
    return response;
}

void response_cleanup(Response response)
{
    free(response.memory);
}
