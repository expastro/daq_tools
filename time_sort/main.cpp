#include <getopt.h>
#include <list>
#include "time_sort.h"

void PrintHelp() {
    
}

void PrintVersion() {
    
}

int main(int argc, char *argv[]) {
    int option;
    
    static struct option options_list[] = {
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
    };


    while ((option = getopt_long(argc, argv, "h:?:v", options_list, 0)) != -1) {
        switch(option) {
            case 'h':
            case '?':
            PrintHelp();
            return 0;
            break;
            case 'v':
            PrintVersion();
            return 0;
            break;
        }
    }
    
    if (optind >= argc) {
        printf("No input file supplied.\n");
        return 1;
    }
    
    while (optind < argc) {
        SortTime(argv[optind]);
    }
    return 0;
}
