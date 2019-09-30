#include "ssl.h"
#include "socket.h"

int init_connection(const char *hostname, int port)
{
    int server = 0;
    server = open_connection(hostname, port);

    return server;
}

SSL* init_ssl_connetion(const char *hostname, int port)
{
    int server;
    SSL *ssl;

    server = init_connection(hostname, port);
    ssl = ssl_create(server);
    return ssl;
}