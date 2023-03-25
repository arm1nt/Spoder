#include <stdio.h>
#include <stdlib.h>
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
static void usage(const char* msg)
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
static void check_option_limit(const char* short_option, const char* long_option, 
    const char* limit, u_int8_t* option_counter)
{
    //TODO: implement.
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
        {"tel", no_argument, NULL, 't'},
        {"email", no_argument, NULL, 'e'},
        {"sort", no_argument, NULL, 's'},
        {"recursive", no_argument, NULL, 'r'},
        0
    };

    //Check that option occurs max. once (or max. given limit if specified otherwise)
    u_int8_t count_v = 0;
    u_int8_t count_o = 0;
    u_int8_t count_p = 0;
    u_int8_t count_t = 0;
    u_int8_t count_e = 0;
    u_int8_t count_s = 0;
    u_int8_t count_r = 0;

    u_int8_t is_verbose = 0;
    u_int8_t filter_tel = 0;
    u_int8_t filter_email = 0;
    u_int8_t sort_output = 0;
    u_int8_t search_recursive = 0;
    u_int8_t custom_port_provided = 0;
    char* port = "80";
    char* output_file = NULL;

    char* option_limit_err_msg = "Option, -%s, --%s must not be given more than %s";

    //TODO: Refactor: Move checking if option has already occured and incrementing counter to function!
    while ((c = getopt_long(argc, argv, ":hvo:p:tesr", longoptions, longindex)) != -1) {
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
            case 't':
                if (count_t)
                    usage("Option -t, --tel must not be given more than once");
                ++count_t;
                
                filter_tel = 1;
                break;
            case 'e':
                if (count_e)
                    usage("Option -e, --email must not be given more than once");
                ++count_e;
                
                filter_email = 1;
                break;
            case 's':
                if (count_s)
                    usage("Option -s, --sort must not be given more than once");
                ++count_s;
                
                sort_output = 1;
                break;
            case 'r':
                if (count_r)
                    usage("Option -r, --recursive must not be given more than once");
                ++count_r;
                
                search_recursive = 1;
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

    char* node = extract_node(stripped_url);

    //TODO: extract path from url

    //create hint struct and struct that will store the result from getaddrinfo

    //create socket

    //connect

    //etc.


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