#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "utilities.h"

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
    
    exit(EXIT_SUCCESS);
}

/**
 * @brief Inform the user about the problem with how he called the program,
 *  display the help menu and exit.
 * 
 * @param msg Information about the problem.
 */
static void usage(const char* msg)
{
    fprintf(stderr, "[ERROR]: %s\n", msg);
    help();
}

/**
 * @brief Print an error msg, detailing the error and then exiting.
 * 
 * @param msg Message containing information about the problem.
 */
static void error_exit(const char* msg)
{
    fprintf(stderr, "[%s]: %s: %s\n", prog_name, msg, strerror(errno));
    exit(EXIT_FAILURE);
}



int main(int argc, char **argv)
{
    prog_name = argv[0];

    int c;
    int *longindex = NULL;

    static struct option longoptions[] = {
        {"port", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {"verbose", no_argument, NULL, 'v'},
        {"output", required_argument, NULL, 'o'},
        0
    };

    u_int8_t count_v = 0;
    u_int8_t count_o = 0;
    u_int8_t count_p = 0;

    u_int8_t is_verbose = 0;
    u_int8_t custom_port_provided = 0;
    char* port = "80";
    char* output_file = NULL;

    while ((c = getopt_long(argc, argv, ":hvo:p:", longoptions, longindex)) != -1) {
        switch(c) {
            case 'h':
                help();
            case 'p':
                if (count_p)
                    usage("Option -p, --port must not be given more than once");

                ++count_p;

                const char* invalid_port_msg = "Port must be a positive integer between 0 and 65535";

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
                if (count_o)
                    usage("Option -o, --output must not be given more than once");

                ++count_o;

                output_file = strdup(optarg);
                break;
            case 'v':
                if (count_v)
                    usage("Option -v, --verbose must not be given more than once");

                ++count_v;
                is_verbose = 1;
                break;
            case '?':
                usage("Invalid option provided");
            case ':':
                usage("Requiered argument for is missing");
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

    return EXIT_SUCCESS;
}