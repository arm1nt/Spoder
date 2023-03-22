#include <stdio.h>
#include <string.h>
#include <errno.h>

short check_url_protocol(const char* url);

char* url_without_protocol(const char* url);