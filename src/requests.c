
#include "requests.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "log_utils.h"

#define CATCH(x) if (x != 0) { return request_result_init_error(x); };

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct Response *mem = (struct Response *)userp;

    char *ptr = realloc(mem->content, mem->content_size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        LOG_ERROR("not enough memory (realloc returned NULL)");
        return 0;
    }

    mem->content = ptr;
    memcpy(&(mem->content[mem->content_size]), contents, realsize);
    mem->content_size += realsize;
    mem->content[mem->content_size] = 0;

    return realsize;
}

struct RequestResult request(
    const char *method,
    const char *url,
    const char **headers,
    const size_t headers_count
)
{
    TRACE_START();
    struct Response response = response_init();
    CURL* curl;
    CATCH(request_init(&curl));

    LOG_DEBUG("Creating headers list");
    struct curl_slist *headerlist = curl_slist_append(NULL, "User-Agent: libcurl-agent/1.0");
    for (size_t i = 0; i < headers_count; i++)
    {
        headerlist = curl_slist_append(headerlist, headers[i]);
    }
    CATCH(headerlist == NULL ? REQUEST_RESULT_INIT_ERROR_HEADERS : 0);
    CATCH(request_prepare(curl, method, url, headerlist, &response));
    CATCH(request_perform(curl));

    long status_code = request_get_http_code(curl);

    CATCH(request_cleanup(curl));

    curl_slist_free_all(headerlist);

    TRACE_END();
    return request_result_init(
        status_code,
        response
    );
}

int request_init(CURL **curl)
{
    TRACE_START();
    LOG_DEBUG("Initializing curl");
    *curl = curl_easy_init();
    if (*curl == NULL)
    {
        LOG_ERROR("Error: curl_easy_init() failed");
        return REQUEST_RESULT_INIT_ERROR_REQUEST_INIT;
    }
    return 0;
}

int request_cleanup(CURL *curl)
{
    TRACE_START();
    if (curl)
    {
        curl_easy_cleanup(curl);
    } else {
        LOG_ERROR("Cannot cleanup curl: curl is NULL");
        return REQUEST_RESULT_INIT_ERROR_REQUEST_CLEANUP;
    }
    TRACE_END();
    return 0;
}

int request_prepare(CURL *curl, const char *method, const char *URL, const struct curl_slist *headers, const struct Response *response)
{
    TRACE_START();
    LOG_DEBUG("Setting url to %s", URL);
    curl_easy_setopt(curl, CURLOPT_URL, URL);
    LOG_DEBUG("Setting method to %s", method);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
    LOG_DEBUG("Setting verbose to 0");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    LOG_DEBUG("Setting header to 0");
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

    if (headers != NULL)
    {
        LOG_DEBUG("Setting headers");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    if (response != NULL)
    {
        LOG_DEBUG("Setting write function");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        LOG_DEBUG("Setting write data");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
    }

    TRACE_END();
    return 0;
}

int request_perform(CURL *curl)
{
    TRACE_START();
    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK)
    {
        LOG_ERROR("Error: curl_easy_perform() failed: %s", curl_easy_strerror(code));
        return REQUEST_RESULT_INIT_ERROR_REQUEST_PERFORM;
    }
    TRACE_END();
    return 0;
}

long request_get_http_code(CURL *curl)
{
    TRACE_START();
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    LOG_DEBUG("Response code: %ld", response_code);
    TRACE_END();
    return response_code;
}

struct RequestResult request_multipart(
    const char *method,
    const char *url,
    const char **headers,
    const size_t headers_count,
    const struct MultipartData *data,
    const size_t data_count
)
{
    TRACE_START();
    CURL* curl;
    CATCH(request_init(&curl));

    CURLM *multi_handle = curl_multi_init();
    curl_mime *form = curl_mime_init(curl);

    struct Response response = response_init();

    LOG_DEBUG("Creating headers list");
    struct curl_slist *headerlist;
    headerlist = curl_slist_append(headerlist, "User-Agent: libcurl-agent/1.0");
    headerlist = curl_slist_append(headerlist, "Expect:");
    for (size_t i = 0; i < headers_count; i++)
    {
        headerlist = curl_slist_append(headerlist, headers[i]);
    }
    CATCH(headerlist == NULL ? REQUEST_RESULT_INIT_ERROR_HEADERS : 0);
    CATCH(request_prepare(curl, method, url, headerlist, &response));

    LOG_DEBUG("Adding form data");
    for (size_t i = 0; i < data_count; i++)
    {
        curl_mimepart *part = curl_mime_addpart(form);
        curl_mime_name(part, data[i].name);
        curl_mime_data(part, data[i].data, CURL_ZERO_TERMINATED);
    }
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
    curl_multi_add_handle(multi_handle, curl);

    LOG_DEBUG("Performing request");
    int still_running = 0;
    do {
        CURLMcode mc = curl_multi_perform(multi_handle, &still_running);

        if(still_running)
        /* wait for activity, timeout or "nothing" */
        mc = curl_multi_poll(multi_handle, NULL, 0, 1000, NULL);

        if(mc)
            break;
    } while(still_running);
 
    long status_code = request_get_http_code(curl);
    LOG_DEBUG("Status code: %ld", status_code);

    LOG_DEBUG("Cleaning up");
    curl_multi_cleanup(multi_handle);
    CATCH(request_cleanup(curl));
    curl_mime_free(form);
    curl_slist_free_all(headerlist);

    TRACE_END();
    return request_result_init(
        status_code,
        response
    );
}
