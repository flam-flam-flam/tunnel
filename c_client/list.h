// #include <stdio.h>
// #include <string.h>

typedef struct connectData
{
    /* data */
    int controlFd;
    SSL_CTX *ctx;
    SSL *ssl;
    char buf[1024 * 8];
    struct connectData *next;
}NODE;



