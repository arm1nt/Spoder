#include "utilities.h"


/**
 * @brief Print an error msg, detailing the error and then exiting.
 * 
 * @param msg Message containing information about the problem.
 */
void error_exit(const char *msg)
{
    fprintf(stderr, "[ERROR]: ./spoder: %s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}

/**
 * @brief Prints only a custom error msg and then exits.
 * 
 * @param msg Custom error msg to be printed.
 */
void error_exit_custom(const char *msg)
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
void regex_error_exit(int error_code, const char *function, regex_t *regex)
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
short check_url_protocol(const char *url)
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
char *url_without_protocol(const char *url)
{
    if (strncmp(url, "http://", strlen("http://")) == 0)
        return strdup(&url[7]);

    if (strncmp(url, "https://", strlen("https://")) == 0)
        return strdup(&url[8]);

    return NULL;
}

/**
 * @brief Checks if the given URL is valid or not.
 * 
 * @param url url to be checked.
 */
void check_valid_url(const char *url)
{
    char *regex_expression = "[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,63}(/[^[:space:]]*)?$";

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
}

/**
 * @brief Find the character that terminates the host name in the specified url
 * 
 * @param url url of which the character that terminates its contained hostname should be found
 * @return char* pointer to the char that occurs first out of '/', '?' and '#'. If none of these
 *  chars is found in the url, NULL is returned.
 */
static char *host_terminating_character(const char *url)
{

    int question_mark_index = INT_MAX;
    int slash_index = INT_MAX;
    int hashtag_index = INT_MAX;

    char *question_mark = strchr(url, '?');
    if (question_mark)
        question_mark_index = (int) (question_mark - url);

    char *hashtag = strchr(url, '#');
    if (hashtag)
        hashtag_index = (int) (hashtag - url);

    char *slash = strchr(url, '/');
    if (slash)
        slash_index = (int) (slash - url);

    if (question_mark_index < slash_index && question_mark_index < hashtag_index)
        return question_mark;
    
    
    if (hashtag_index < question_mark_index && hashtag_index < slash_index)
        return hashtag;
        

    if ((slash_index < question_mark_index) && (slash_index < hashtag_index))
        return slash;

    return NULL;
}

/**
 * @brief Extract node from the url (e.g. www.example.com/f1/f2/f3 -> www.example.com)
 * 
 * @param url URL from which the node should be extracted.
 * @return char* the extracted node. The node has to be freed.
 */
char *extract_node(const char *url)
{
    char *node;
    char *delimeter = host_terminating_character(url);

    if (delimeter != NULL) {
        u_int32_t index = (int) (delimeter - url);
        node = malloc(sizeof(char) * (index+1));

        if (!node)
            error_exit("malloc failed when extracting node");
        
        memset(node, '\0', sizeof(node));
        strncpy(node, url, index);
    } else {
        node = strdup(url);
    }

    //Only search for '/' to end the host name
    /*if (strchr(url, '/') != NULL) {
        int index_of_first_slash = (int) ((strchr(url, '/')) - url);
        node = malloc(sizeof(char) * (index_of_first_slash+1));

        if (!node)
            error_exit("malloc failed when extracting the node");

        memset(node, '\0', sizeof(node));
        strncpy(node, url, index_of_first_slash);
    } else {
        node = strdup(url);
    }*/

    return node;
}

/**
 * @brief 
 * 
 * @param url 
 * @return char* Returns path, if no path is given '/' is returned to request the root
 */
char *extract_path(const char *url)
{
    char *path;

    //TODO: implement
}

//TODO: also pass an array where you can define custom headers
//for required headers, check if there custom ones have been passed.
char *create_http_header(const char *service, const char **cusotm_headers)
{
    return NULL;
}