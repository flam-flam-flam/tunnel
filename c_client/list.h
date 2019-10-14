// #include <stdio.h>
// #include <string.h>
#include "ssl.h"
#include "ae.h"

typedef struct connectData
{
    /* data */
    int Fd;
    int TunnelID;
    int64_t clusterConnectinID;
    SSL_CTX *ctx;
    SSL *ssl;
    char buf[1024 * 8];
    struct connectData *next;
} NODE;


// NODE* initNode(int Fd,SSL_CTX *ctx, SSL *ssl, char *buf, int64_t clusterConnectinID);
NODE* initNode(int Fd,SSL_CTX *ctx, SSL *ssl, char *buf, int64_t clusterConnectinID,int TunnelID);
// NODE* createLastNode(NODE* phead, int fd, SSL_CTX *ctx, SSL *ssl, char *buf, int64_t clusterConnectinID);
NODE* createLastNode(NODE* phead, int fd, SSL_CTX *ctx, SSL *ssl, char *buf, int64_t clusterConnectinID,int TunnelID);
NODE* searchNode(NODE* phead,int key);//key is fd
NODE* deleteOneNode(NODE* phead,int key);
NODE* searchAnotherNode(NODE* phead,int key,int64_t clusterConnectinID);
NODE* deleteAnotherNode(NODE* phead,int key,int64_t clusterConnectinID);
// NODE* searchSelfNode(NODE* phead,int key,int64_t clusterConnectinID);