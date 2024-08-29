#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

int main(int argc, char *argv[]) {
    int port = 80;
    const char *address = "127.0.0.1";
    const char *path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--address") == 0 && i + 1 < argc) {
            address = argv[++i];
        } else if (strcmp(argv[i], "--path") == 0 && i + 1 < argc) {
            path = argv[++i];
        } else {
            return EXIT_FAILURE;
        }
    }

    if (path == NULL) {
        fprintf(stderr, "Path to HTML files is required.\n");
        return EXIT_FAILURE;
    }

    if (start_server(address, port, path) != 0) {
        fprintf(stderr, "Server failed to start.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
