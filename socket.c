#include "socket.h"

/**
 * @brief Make connection based on node and port.
 * 
 * @param node node (e.g. www.example.com)
 * @param port port (e.g. 443)
 * @return int number > 0 (filedescriptor) if connection
 *  was established; -1 if unable to establish connection.
 */
int establish_connection(const char *node, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int getaddrinfo_res = getaddrinfo(node, port, &hints, &result);
    if (getaddrinfo_res != 0) {
        fprintf(stderr, "[ERROR]: ./spoder: getaddrinfo failed: %s\n", gai_strerror(getaddrinfo_res));
        exit(EXIT_FAILURE);
    }

    int socket_fd = -1;
    struct addrinfo *rp;
    for(rp = result; rp != NULL; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (socket_fd == -1)
            continue;

        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;
    }

    if (rp == NULL)
        socket_fd = -1;

    freeaddrinfo(result);

    return socket_fd;
}