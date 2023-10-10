
#ifndef REQUEST_RESULT_H
#define REQUEST_RESULT_H

#include <curl/curl.h>
#include "response.h"

struct RequestResult {
    const int error_code;
    const long status_code;
    struct Response response;
};

extern struct RequestResult request_result_init(const long status_code, const struct Response response);
extern struct RequestResult request_result_init_error(const int error_code);
extern void request_result_cleanup(struct RequestResult *result);

#endif // REQUEST_RESULT_H