#ifndef SUB_H
#define SUB_H

#include "cloudflare.h"
#include "parameters.h"
#include "requests.h"
#include "response.h"
#include "request_result.h"

#define IP_HAYSTACK "\nip="

#define CATCH(x) if (x.error_code != 0) { return x.error_code; }

extern int sub_get_ip(char *ip);
extern int sub_handle_zones(const struct Parameters params, const CloudFlare cloudflare, const struct Response response);

#endif