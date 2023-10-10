
#ifndef REQUESTS_H
#define REQUESTS_H

#include <curl/curl.h>
#include "request_result.h"

struct MultipartData {
    const char *name;
    const char *data;
};

extern long request_get_http_code(CURL *curl);

// Function to initialize curl
extern int request_init(CURL **curl);

// Function clean up curl
extern int request_cleanup(CURL *curl);

// Function to prepare the request
extern int request_prepare(CURL *curl, const char *method, const char *URL, const struct curl_slist *headers, const struct Response *response);

// Function to perform the request
extern int request_perform(CURL *curl);

extern struct RequestResult request(const char *method, const char *url, const char **headers, const size_t headers_count);

extern struct RequestResult request_multipart(
    const char *method,
    const char *url,
    const char **headers,
    const size_t headers_count,
    const struct MultipartData *data,
    const size_t data_count
);

#endif /* REQUESTS_H */