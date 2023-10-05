
#ifndef REQUEST_RESULT_H
#define REQUEST_RESULT_H

#include <curl/curl.h>
#include "response.h"

#define REQUEST_RESULT_INIT_ERROR_REQUEST_INIT -1
#define REQUEST_RESULT_INIT_ERROR_REQUEST_PERFORM -2
#define REQUEST_RESULT_INIT_ERROR_HEADERS -3
#define REQUEST_RESULT_INIT_ERROR_REQUEST_CLEANUP -4

struct RequestResult {
    const int error_code;
    const long status_code;
    struct Response response;
};

extern struct RequestResult request_result_init(const long status_code, const struct Response response);
extern struct RequestResult request_result_init_error(const int error_code);
extern void request_result_cleanup(struct RequestResult *result);

#endif // REQUEST_RESULT_H