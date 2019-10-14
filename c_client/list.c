#include <stdio.h>
#include "list.h"


NODE* initNode(int Fd,SSL_CTX *ctx, SSL *ssl, char *buf, int64_t clusterConnectinID,int TunnelID) {
    NODE* pnode = (NODE *)malloc(sizeof(NODE));
    memset(pnode, '\0', sizeof(NODE));
    memset(pnode->buf, 0,sizeof(pnode->buf));
    pnode->Fd = Fd;
    pnode->clusterConnectinID = clusterConnectinID;
    pnode->TunnelID = TunnelID;
    if(ctx) {
        pnode->ctx = ctx;
    }
    else {
        pnode->ctx = NULL;
    }
    
    if(ssl) {
        pnode->ssl = ssl;
    }
    else {
        pnode->ssl = NULL;
    }
    
    if(buf) {
        strcpy(pnode->buf,buf);
    }
   
    pnode->next = NULL;
    return pnode;
}



//?????????
NODE* createLastNode(NODE* phead, int fd, SSL_CTX *ctx, SSL *ssl, char *buf, int64_t clusterConnectinID,int TunnelID) {
    NODE* newNode = initNode(fd,ctx,ssl,buf,clusterConnectinID,TunnelID);
    if(phead){
        NODE* ptemp = phead;
        while(ptemp && ptemp->next){
            ptemp = ptemp->next; //????????
        }
        ptemp->next = newNode;
    }else{
        return newNode;
    }
    return phead;
}

//???? key is fd
NODE* searchNode(NODE* phead,int key) {
    NODE* ptemp = phead;
    
    if(ptemp){
        while(ptemp){
            if(ptemp->Fd && ptemp->Fd == key){
                return ptemp;
            }
            ptemp = ptemp->next;
        }
    }else{
        return NULL;
    }
    return NULL;
}


/*NODE* searchSelfNode(NODE* phead,int key,int64_t clusterConnectinID) {
    NODE* ptemp = phead;
    
    if(ptemp){
        while(ptemp){
            if(ptemp->Fd &&  pnode->clusterConnectinID == clusterConnectinID && ptemp->Fd == key){
                return ptemp;
            }
            ptemp = ptemp->next;
        }
    }else{
        return NULL;
    }
    return NULL;
}*/

//???? key is fd
NODE* searchAnotherNode(NODE* phead,int key,int64_t clusterConnectinID) {
    NODE* ptemp = phead;
    
    if(ptemp){
        while(ptemp){
            if(ptemp->Fd && ptemp->clusterConnectinID == clusterConnectinID && ptemp->Fd != key){
                return ptemp;
            }
            ptemp = ptemp->next;
        }
    }else{
        return NULL;
    }
    return NULL;
}

//??????
NODE* deleteOneNode(NODE* phead,int key) {
    if(phead){
        NODE* temp = phead;
        if(temp->Fd && temp->Fd == key) { //???????
            SSL_shutdown(temp->ssl);
            SSL_free(temp->ssl);  
            SSL_CTX_free(temp->ctx);
            close(temp->Fd);
            free(temp);
            temp = NULL;
        } else {
            while(temp && temp->next) {
                NODE* backNode = temp->next;
                if(backNode->Fd && backNode->Fd == key) {
                    temp->next = backNode->next;
                    SSL_shutdown(backNode->ssl);
                    SSL_free(backNode->ssl);  
                    SSL_CTX_free(backNode->ctx);
                    close(backNode->Fd);
                    free(backNode);
                    backNode = NULL;
                    return phead;
                }
                temp = temp->next;
            }
        }
    } else{
        printf("sorry, your linkList is NULL");
        return NULL;
    }
    return phead;
}

NODE* deleteAnotherNode(NODE* phead,int key,int64_t clusterConnectinID) {
    if(phead){
        NODE* temp = phead;
        if(temp->Fd && temp->clusterConnectinID == clusterConnectinID && temp->Fd != key) { //???????
            SSL_shutdown(temp->ssl);
            SSL_free(temp->ssl);  
            SSL_CTX_free(temp->ctx);
            close(temp->Fd);
            free(temp);
            temp = NULL;
        } else {
            while(temp && temp->next) {
                NODE* backNode = temp->next;
                if(backNode->Fd && backNode->clusterConnectinID == clusterConnectinID && backNode->Fd != key) {
                    temp->next = backNode->next;
                    SSL_shutdown(backNode->ssl);
                    SSL_free(backNode->ssl);  
                    SSL_CTX_free(backNode->ctx);
                    close(backNode->Fd);
                    free(backNode);
                    backNode = NULL;
                    return phead;
                }
                temp = temp->next;
            }
        }
    } else{
        printf("sorry, your linkList is NULL");
        return NULL;
    }
    return phead;
}

//???????
/*NODE* createHeadNode(NODE* phead, int data) {
    NODE* newNode = initNode(data);
    NODE* ptemp = phead;
    if(phead){
        phead = newNode; //????????
        newNode->next = ptemp;
    }else{
        return newNode;
    }
    return phead;
}

//????
NODE* insertNodePre(NODE* phead,int key,int data) {
    NODE *newNode = initNode(data);
    if(phead){
        NODE* ptemp = phead;
        if(ptemp->data && ptemp->data == key){ //??????
            phead = newNode;
            newNode->next = ptemp;
        }else{
            while(ptemp && ptemp->next){ 
                NODE* backNode = ptemp->next;
                if(backNode->data && backNode->data == key){ //???????
                    newNode->next = backNode;
                    ptemp->next = newNode;
                    return phead;
                }
                ptemp = ptemp->next;
            }
        }
    }else{
        return newNode;
    }
    return phead;
}

//????
NODE* insertNodeBack(NODE* phead,int key,int data) {
    NODE* newNode = initNode(data);
    if(phead){
        NODE* ptemp = phead;
        if(ptemp->data && ptemp->data == key){//??????
            ptemp->next = newNode;
        }else{
            while(ptemp){
                if(ptemp->data && ptemp->data == key){
                    newNode->next = ptemp->next;
                    ptemp->next = newNode;
                    return phead;
                }
                ptemp = ptemp->next;
            }
        }
    }else{
        return newNode;
    }
    return phead;
}

//??????
int linkLength(NODE* phead){
    int n = 0;
    if(phead){
        NODE* temp = phead;
        while(temp){
            n = n+1;
            temp = temp->next;
        }
    }
    return n;
}

*/