#include "response.h"
#include "log_utils.h"

struct Response response_init()
{
    TRACE_START();
    LOG_DEBUG("Initializing response");
    struct Response response = {
        .content = malloc(1),
        .content_size = 0
    };
    TRACE_END();
    return response;
}

void response_cleanup(struct Response *response)
{
    TRACE_START();
    LOG_DEBUG("Freeing response content");
    free(response->content);
    response->content = NULL;
    TRACE_END();
}