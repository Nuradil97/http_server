#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define BUFFER_SIZE 4096

// Function to handle client requests
static void handle_client(SOCKET client_socket, const char *file_path) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        closesocket(client_socket);
        return;
    }

    buffer[bytes_received] = '\0'; // Null-terminate the buffer
    char *file_name = strtok(buffer, " ");
    file_name = strtok(NULL, " ");

    if (strstr(file_name, ".html") == NULL) {
        const char *response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_socket, response, strlen(response), 0);
        closesocket(client_socket);
        return;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s", file_path, file_name);

    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        const char *response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_socket, response, strlen(response), 0);
    } else {
        const char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client_socket, header, strlen(header), 0);

        size_t n;
        while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(client_socket, buffer, n, 0);
        }

        fclose(file);
    }

    closesocket(client_socket);
}

// Function to start the server
int start_server(const char *address, int port, const char *path) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock\n");
        return -1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    if (listen(server_socket, 10) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    printf("Server started at %s:%d\n", address, port);

    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            continue;
        }

        handle_client(client_socket, path);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}


// Main function to handle command-line arguments and start the server
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