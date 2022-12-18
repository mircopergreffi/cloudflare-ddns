
#include <curl/curl.h>

/* https://curl.se/libcurl/c/CURLOPT_READFUNCTION.html */
struct memory {
	char *response;
	size_t size;
};
static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct memory *mem = (struct memory *)userp;
	
	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if(ptr == NULL)
		return 0;  /* out of memory! */
	
	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;
	
	return realsize;
}
/* ----------------------------------------------------- */

char * http_request(CURL *curl, )
{
	struct memory chunk = {0};
	// send all data to this function
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
	// we pass our 'chunk' struct to the callback function
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	

	return chunk.response;
}