#include "utilities.h"


/**
 * @brief Print an error msg, detailing the error and then exiting.
 * 
 * @param msg Message containing information about the problem.
 */
void error_exit(const char* msg)
{
    fprintf(stderr, "[./spoder]: %s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}


/**
 * @brief Prints only a custom error msg and then exits.
 * 
 * @param msg Custom error msg to be printed.
 */
void error_exit_custom(const char* msg)
{
    fprintf(stderr, "[ERROR]: ./spoder: %s\n", msg);
    exit(EXIT_FAILURE);
}


/**
 * @brief Prints an error msg regarding an regex error, detailing the error and then exiting.
 * 
 * @param error_code error code given by the used regex function
 * @param function regex function in which the error occured
 * @param msg 
 */
void regex_error_exit(int error_code, const char* function, regex_t* regex)
{
    char msgbuffer[100];

    regerror(error_code, regex, msgbuffer, 100);

    if (regex != NULL)
        regfree(regex);
    
    fprintf(stderr, "%s() failed with %s\n", function, msgbuffer);
    exit(EXIT_FAILURE);
}


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


/**
 * @brief Extract node from the url (e.g. www.example.com/f1/f2/f3 -> www.example.com)
 * 
 * @param url URL from which the node should be extracted.
 * @return char* the extracted node. The node has to be freed.
 */
char* extract_node(const char* url)
{
    char* node;

    if (strchr(url, '/') != NULL) {
        int index_of_first_slash = (int) ((strchr(url, '/')) - url);
        node = malloc(sizeof(char) * (index_of_first_slash+1));

        if (!node)
            error_exit("malloc failed when extracting the node");

        memset(node, '\0', sizeof(node));
        strncpy(node, url, index_of_first_slash);
    } else {
        node = strdup(url);
    }

    char* regex_expression = "[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,63}(/[^[:space:]]*)?$";

    regex_t regex;
    int rc;

    if ((rc = regcomp(&regex, regex_expression, REG_EXTENDED)) != 0)
        regex_error_exit(rc, "regcomp", &regex);

    int result = regexec(&regex, url, 0, 0, 0);

    regfree(&regex);

    if (result == REG_NOMATCH) {
        error_exit_custom("Invalid URL given - malformed node");
    } else if(result < 0) {
        regex_error_exit(result, "regexec", NULL);
    }

    return node;
}