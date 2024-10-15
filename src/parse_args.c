#include <stdio.h>
#include "ping_session.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "resources.h"
#include "resources.h"
#include "exit_constants.h"

#include <stdlib.h> //get_long_opt
#include <getopt.h> //get_long_opt


static int parse_positive_int(char *str_value, char *program_name)
{
    int value = -1;
    value = atoi(str_value);
    
    if (value > 0 && value <= INT_MAX)
    {
        return value;
    } 
    else if (value < 0) 
    {
        fprintf(stderr, "%s: option value too big: %s\n",program_name, str_value);
    }
    else if (value == 0)
    {
        fprintf(stderr, "%s: option value too small: %s\n",program_name, str_value);
    }
    exit(EXIT_FAILURE);
}

void parse_args(int ac, char **argv)
{
    int option_index = 0;

    // Define the long options
    struct option long_options[] = {
        {"ttl", required_argument, 0, 't'},
        {"verbose", no_argument, 0, 'v'},
        {"debug", no_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {"usage", no_argument, 0, 'u'},
        {"count", required_argument, 0, 'c'},
        {"timeout", required_argument, 0, 'w'},
        {"linger", required_argument, 0, 'W'},
        {0, 0, 0, 0}
    };

    while (1) {
        int c = getopt_long(ac, argv, "vd?hut:c:w:W:", long_options, &option_index);

        // End of options
        if (c == -1) {
            break;
        }

        switch (c) 
        {
            case '?': //we encountered unknown option or option -?
                if (optopt != 0) //the variable keeps the unknown option. If it is 0 we encountered -? option, which is the same as help (h) and usage (u)
                {
                    //if optopt != 0, then we encountered unknown flag. Exit in this case.
                    //lib prints the unknown flag error
                    fprintf(stderr, "Try '%s --help' or '%s --usage' for more information.\n",  argv[0], argv[0]);
                    exit(EXIT_ERROR);
                }
                // fall through
            case 'h':
                // fall through
            case 'u':
                g_ping_session.flags.print_man_only = true;
                c = -1; //stop parsing flags error messages about unknown flags are not printed.
                break;
            case 't':
                g_ping_session.flags.ttl = parse_positive_int(optarg, argv[0]);
                break;
            case 'c':
                g_ping_session.flags.count = parse_positive_int(optarg, argv[0]);
                break;
            case 'w':
                g_ping_session.flags.timeout = parse_positive_int(optarg, argv[0]);
                break;
            case 'W':
                g_ping_session.flags.linger.tv_sec = parse_positive_int(optarg, argv[0]);
                break;
            case 'v':
                g_ping_session.flags.is_verbose = true;
                break;
            case 'd':
                g_ping_session.flags.is_debug = true;
                break;
            default:
                abort();
        }
        if (c == -1)
        {
            break;
        }
    }
    
    //arg lib after parsing flags, sorts argv arguments, so first go flags and only then go non flag arguments. 
    //e.g. input was: program_name host1 --ttl=10 host2 -v host3 -> program_name --ttl=10 -v host1 host2 host3
    //optind keeps index of first-non flag argument
    if (optind < ac)
    {
        g_ping_session.target_arg = argv[optind];
    }
    else if (g_ping_session.flags.print_man_only)
    {
        return;
    }
    else
    {
        fprintf(stderr, "%s: missing host operand\n", argv[0]);
        exit(EXIT_ERROR);
    }
}
