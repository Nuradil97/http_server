#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>

int start_server(const char *address, int port, const char *path);

#endif