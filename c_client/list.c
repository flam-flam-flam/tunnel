#include <stdio.h>

NODE* initNode(int controlFd,SSL_CTX *ctx,SSL *ssl,char *buf) {
    NODE* pnode = (NODE *)malloc(sizeof(NODE));
    pnode->controlFd = controlFd;
    pnode->ctx = ctx;
    pnode->ssl = ssl;
    if(buf && length(buf))
    {
        strcpy(pnode->buf,buf)
    }
    pnode->next = NULL;
    return pnode;
}