#include <getopt.h>
#include <iostream>
#include <list>
#include "time_sort.h"

constexpr char VERSION[] = "1.0";

void PrintHelp(char* program_name) {
    std::cout << "Usage:\n";
    std::cout << program_name << " [options] input_filename\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "-h, --help display this help and exit\n";
    std::cout << "-v, --version  output version information and exit\n";
    
}

void PrintVersion(char* program_name) {
    std::cout << program_name << " v" << VERSION << std::endl;
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
            PrintHelp(argv[0]);
            return 0;
            break;
            case 'v':
            PrintVersion(argv[0]);
            return 0;
            break;
        }
    }
    
    if (optind >= argc) {
        std::cout << "No input file supplied.\n";
        PrintHelp(argv[0]);
        return 1;
    }
    
    while (optind < argc) {
        SortTime(argv[optind]);
    }
    return 0;
}
