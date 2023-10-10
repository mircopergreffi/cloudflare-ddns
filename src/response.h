#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdlib.h>

// Holds a response from a request
struct Response {
    char *content;
    size_t content_size;
};

// Initializes a response
extern struct Response response_init();

// Cleans up a response
extern void response_cleanup(struct Response *response);

#endif // RESPONSE_H