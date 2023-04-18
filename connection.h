#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <openssl/x509.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include "utilities.h"

int establish_connection(const char *node, const char *port);

SSL_CTX *initialize_ssl_context(void);

int create_ssl_connection(SSL **ssl, SSL_CTX *ctx, u_int32_t socket_fd);