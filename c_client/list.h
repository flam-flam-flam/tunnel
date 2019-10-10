// #include <stdio.h>
// #include <string.h>
#include "ssl.h"
#include "ae.h"

typedef struct connectData
{
    /* data */
    int controlFd;
    SSL_CTX *ctx;
    SSL *ssl;
    char buf[1024 * 8];
    struct connectData *next;
}NODE;

// NODE* initNode(int controlFd);
NODE* initNode(int controlFd,SSL_CTX *ctx, SSL *ssl, char *buf);
NODE* createLastNode(NODE* phead, int fd, SSL_CTX *ctx, SSL *ssl, char *buf);
NODE* searchNode(NODE* phead,int key);//key is fd
NODE* deleteOneNode(NODE* phead,int key);