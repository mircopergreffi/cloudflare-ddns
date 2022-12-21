
#ifndef REQUESTS_H
#define REQUESTS_H

#include <curl/curl.h>

// Function to initialize curl
CURL * request_init(void);

// Function clean up curl
void request_cleanup(CURL *curl);

// Function to prepare the request
void request_prepare(CURL *curl, char *method, char *URL);

// Function to set one header
void request_set_header(CURL *curl, char *header);

// Function to perform the request
CURLcode request_perform(CURL *curl);

#endif /* REQUESTS_H */