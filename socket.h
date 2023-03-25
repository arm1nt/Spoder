#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "utilities.h"

int establish_connection(const char *node, const char *port);