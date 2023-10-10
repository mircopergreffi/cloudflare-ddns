#include "request_result.h"
#include "log_utils.h"

struct RequestResult request_result_init(const long status_code, const struct Response response)
{
    TRACE_START();
    struct RequestResult result = {
        .error_code = 0,
        .status_code = status_code,
        .response = response
    };
    TRACE_END();
    return result;
}

struct RequestResult request_result_init_error(const int error_code)
{
    TRACE_START();
    struct RequestResult result = {
        .error_code = error_code,
        .status_code = 0,
        .response = {
            .content = NULL,
            .content_size = 0
        }
    };
    TRACE_END();
    return result;
}

void request_result_cleanup(struct RequestResult *result)
{
    TRACE_START();
    if (result->response.content != NULL)
    {
        response_cleanup(&result->response);
    }
    TRACE_END();
}