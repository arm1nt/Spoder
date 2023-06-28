#ifndef LIBUTILITIES
#define LIBUTILITIES

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <stdlib.h>
#include <limits.h>

typedef struct TextBuffer {
    char *data;
    size_t available_size;
    size_t used_size;
} TextBuffer;

void error_exit(const char* msg);

void error_exit_custom(const char* msg);

void regex_error_exit(int error_code, const char* function, regex_t* regex);

short check_url_protocol(const char* url);

void check_valid_url(const char *url);

char* url_without_protocol(const char* url);

char* extract_node(const char* url);

short search_for_tag_end(char *buffer, u_short buffer_counter);

#endif