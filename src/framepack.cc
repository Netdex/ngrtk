
#include <iostream>

#include <getopt.h>

void print_usage(int argc, char *argv[]){
    std::cerr << "Usage: " << argv[0] << " -" << std::endl;
}
int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "a:")) != -1) {
        switch (opt) {
            case 'a':
                std::cout << "TEST" << std::endl;
                break;
            default:
                print_usage(argc, argv);
                exit(EXIT_FAILURE);
        }
    }
    if (optind >= argc) {
        print_usage(argc, argv);
        exit(EXIT_FAILURE);
    }
    printf("name argument = %s\n", argv[optind]);
    return 0;
}