
#ifndef REQUESTS_H
#define REQUESTS_H

#include <curl/curl.h>

struct Response_T {
    char *memory;
    size_t size;
};
typedef struct Response_T Response;

// Function to initialize curl
extern CURL * request_init(void);

// Function clean up curl
extern void request_cleanup(CURL *curl);

// Function to prepare the request
extern void request_prepare(CURL *curl, const char *method, const char *URL, const Response *response);

// Function to set one header
extern void request_set_header(CURL *curl, const char *header);

// Function to perform the request
extern CURLcode request_perform(CURL *curl);

extern Response request(const char *method, const char *url, const char **headers, const size_t headers_count);

extern Response response_init();
extern void response_cleanup(Response response);

#endif /* REQUESTS_H */