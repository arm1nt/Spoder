#include "utilities.h"


/**
 * @brief Check that the given URL uses a valid protocol.
 *  Accepted protocols are:
 *      - http
 *      - https
 * 
 * @param url 
 * @return short returns a positive number if protocol is accepted and otherwise a negative number
 */
short check_url_protocol(const char* url)
{

    if (strncmp(url, "http://", strlen("http://")) == 0 || strncmp(url, "https://", strlen("https://")) == 0)
        return 1;

    return -1;
}

/**
 * @brief Return the URL without the protocol, e.g. http://example.com -> return: example.com
 *      If the protocol is not recognized, NULL is returned
 * 
 * @param url url of which the protocoll should be stripped
 * @return char* url without protocol
 */
char* url_without_protocol(const char* url)
{
    if (strncmp(url, "http://", strlen("http://")) == 0)
        return strdup(&url[7]);

    if (strncmp(url, "https://", strlen("https://")) == 0)
        return strdup(&url[8]);

    return NULL;
}