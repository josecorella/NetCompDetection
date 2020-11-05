#include <errno.h> 

#include "parse_json.h"

int main (int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "NOT ENOUGH ARGUMENTS PROVIDED\n");
        return EXIT_FAILURE;
    }

    struct json config;

    read_json(&config, argv[1]);
    print_json_s(&config);
}
