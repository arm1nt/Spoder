#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "utilities.h"
#include "connection.h"

char *prog_name;


/**
 * @brief Print a complete list of all the options this programs takes and terminate.
 * 
 */
static void help(void)
{
    printf("USAGE: %s [OPTION]... URL\n\n", prog_name);

    printf("\t -h, --help \t\t Display this help and exit.\n");
    printf("\t -p, --port \t\t Specify port to be used, if not provided 80 is used as default.\n");
    printf("\t -v, --verbose \t\t Verbose mode: Display more information.\n");
    printf("\t -o, --output \t\t Specify output file, if not provided stdout is used as default.\n");
    printf("\t -e, --email \t\t Also search for email addresses.\n");
    printf("\t -t, --tel \t\t Also search for phone numbers.\n");
    printf("\t -s, --sort \t\t Sort output by category (tel number, email, link).\n");
    printf("\t -r, --recursive \t Follow found links.\n");
    
    exit(EXIT_SUCCESS);
}

/**
 * @brief Inform the user about the problem with how he called the program,
 *  display the help menu and exit.
 * 
 * @param msg Information about the problem.
 */
static void usage(const char *msg)
{
    fprintf(stderr, "[ERROR]: %s\n", msg);
    help();
}

/**
 * @brief Checks that the option has not been provided more often than the specified limit.
 *  If the option occurs more often than the specified limit, the usage function is called.
 * 
 * @param short_option short form of the option that has been provided (e.g. -h)
 * @param long_option  long form of the option that has been provided (e.g. --help)
 * @param limit specifies how often the option can occur at most.
 * @param option_counter how often the option has occured so far. If option_counter < limit, then
 *  the option_counter is incremented.
 */
static void check_option_limit(const char *short_option, const char *long_option, 
    const char *limit, u_int8_t *option_counter, u_int8_t option_limit)
{
    if (*option_counter >= option_limit) {
        char buffer[1024];
        sprintf(buffer, "Option -%s, --%s must not be given more than %s", short_option, long_option, limit);
        usage(buffer);
    }
    (*option_counter)++;
}



int main(int argc, char **argv)
{
    prog_name = argv[0];

    int c;
    int *longindex = NULL;

    //TODO: Let user pass cookie / session id -> if multiples cookies, cookies as list: cookie1,cookie2,cookie3
    //      username and password if site is password protected (if not specified in the url already)
    //      Allow user to disable URL checkes etc.
    //      Allow user to pass in headers e.g -H "Host: example.host"
    static struct option longoptions[] = {
        {"port", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {"verbose", no_argument, NULL, 'v'},
        {"output", required_argument, NULL, 'o'},
        {"tel", no_argument, NULL, 't'},
        {"email", no_argument, NULL, 'e'},
        {"sort", no_argument, NULL, 's'},
        {"recursive", no_argument, NULL, 'r'},
        0
    };


    u_int8_t count_v = 0;
    u_int8_t count_o = 0;
    u_int8_t count_p = 0;
    u_int8_t count_t = 0; //Maybe rename, _t shouldnt be used as postifx
    u_int8_t count_e = 0;
    u_int8_t count_s = 0;
    u_int8_t count_r = 0;

    u_int8_t is_verbose = 0;
    u_int8_t filter_tel = 0;
    u_int8_t filter_email = 0;
    u_int8_t sort_output = 0;
    u_int8_t search_recursive = 0;
    u_int8_t custom_port_provided = 0;
    char *port = "80";
    char *output_file = NULL;

    while ((c = getopt_long(argc, argv, ":hvo:p:tesr", longoptions, longindex)) != -1) {
        switch(c) {
            case 'h':
                help();
            case 'p':
                check_option_limit("p", "port", "once", &count_p, 1);

                const char *invalid_port_msg = "Port must be a positive integer between 0 and 65535";

                if (*optarg == '\0')
                    usage(invalid_port_msg);

                char *endptr;
                long given_port = strtol(optarg, &endptr, 10);

                if (*endptr != '\0' || given_port > 65535 || given_port < 0)
                    usage(invalid_port_msg);
                
                port = strdup(optarg);
                ++custom_port_provided;
                
                break;
            case 'o':
                check_option_limit("o", "output", "once", &count_o, 1);

                output_file = strdup(optarg);
                break;
            case 'v':
                check_option_limit("v", "verbose", "once", &count_v, 1);

                is_verbose = 1;
                break;
            case 't':
                check_option_limit("t", "tel", "once", &count_t, 1);

                filter_tel = 1;
                break;
            case 'e':
                check_option_limit("e", "email", "once", &count_e, 1);

                filter_email = 1;
                break;
            case 's':
                check_option_limit("s", "sort", "once", &count_s, 1);

                sort_output = 1;
                break;
            case 'r':
                check_option_limit("r", "recursive", "once", &count_r, 1);

                search_recursive = 1;
                break;
            case '?':
                usage("Invalid option provided");
            case ':':
                usage("Required argument is missing");
            default:
                error_exit("There was an error parsing the given options");
        }
    }

    if (argc - optind != 1)
        usage("URL must be given as positional argument");

    char *url = strdup(argv[optind]);


    if (check_url_protocol(url) < 0)
        usage("Invalid protocol given, only accepted protocols are:\n\t- http\n\t- https\n");


    char *stripped_url = url_without_protocol(url);

    check_valid_url(stripped_url);

    char *node = extract_node(stripped_url);

    //TODO: extract path from url


    SSL_CTX *ctx = NULL;
    if (strncmp(port, "443", strlen("443")) == 0) {
        ctx = initialize_ssl_context();

        if (!ctx)
        error_exit("Failed to create SSL context.");
    }
    
    int socket_fd = establish_connection(node, port);

    if (socket_fd == -1)
        error_exit("Failed to connect to service");

    SSL *ssl = NULL;

    if (strncmp(port, "443", strlen("443")) == 0) {
        int result = create_ssl_connection(&ssl, ctx, socket_fd);
        if (result == -1) {
            error_exit("SSL_new failed");
        } else if (result == -2) {
            error_exit("SSL_set_fd failed");
        } else if (result == -3) {
            error_exit("SSL_connect failed");
        }
    }

    //TODO: add certificate validation

    //***********************************
    //***********************************
    char buffer[2048];
    sprintf(buffer, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: spoder\r\n\r\n", node);
    SSL_write(ssl, buffer, strlen("GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: spoder\r\n\r\n") + strlen(node) + 2);

    int bytes_received = 0;

    do {
        bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
        printf("Received (%d bytes): '%s'\n", bytes_received, buffer);
        fflush(stdout);
    } while( bytes_received >= 1);

    //***********************************
    //***********************************
    
    
    //create linked list or mabe stack idk doesnt rly matter ~zzz
    //  this list will hold the initial link and all the links we find when parsing the web page
    //  if there is a node in the list (node contains url to connect to and port)
    //  make a get request and search the page
    //  while (list.hasNext()) {
    //      int fd = establish_connection(list.node, list.port);
    //      
    //      parse etc.
    //  }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(socket_fd);

    if (custom_port_provided) {
        free(port);
        port = NULL;
    }

    if (output_file) {
        free(output_file);
        output_file = NULL;
    }

    free(url);
    url = NULL;
    
    free(node);
    node = NULL;

    return EXIT_SUCCESS;
}