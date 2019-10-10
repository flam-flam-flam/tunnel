#include <stdio.h>
#include "list.h"


NODE* initNode(int controlFd,SSL_CTX *ctx, SSL *ssl, char *buf) {
    NODE* pnode = (NODE *)malloc(sizeof(NODE));
    memset(pnode, '\0', sizeof(NODE));
    memset(pnode->buf, 0,sizeof(pnode->buf));
    pnode->controlFd = controlFd;
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



//β�崴��һ���½ڵ�
NODE* createLastNode(NODE* phead, int fd, SSL_CTX *ctx, SSL *ssl, char *buf) {
    NODE* newNode = initNode(fd,ctx,ssl,buf);
    if(phead){
        NODE* ptemp = phead;
        while(ptemp && ptemp->next){
            ptemp = ptemp->next; //�ҵ����һ���ڵ�
        }
        ptemp->next = newNode;
    }else{
        return newNode;
    }
    return phead;
}



//��ֵ���� key is fd
NODE* searchNode(NODE* phead,int key) {
    NODE* ptemp = phead;
    
    if(ptemp){
        while(ptemp){
            if(ptemp->controlFd && ptemp->controlFd == key){
                return ptemp;
            }
            ptemp = ptemp->next;
        }
    }else{
        return NULL;
    }
    return NULL;
}

//ɾ��һ���ڵ�
NODE* deleteOneNode(NODE* phead,int key) {
    if(phead){
        NODE* temp = phead;
        if(temp->controlFd && temp->controlFd == key) { //ɾ����һ���ڵ�
            SSL_shutdown(temp->ssl);
            SSL_free(temp->ssl);  
            SSL_CTX_free(temp->ctx);
            close(temp->controlFd);
            free(temp);
            temp = NULL;
        } else {
            while(temp && temp->next) {
                NODE* backNode = temp->next;
                if(backNode->controlFd && backNode->controlFd == key) {
                    temp->next = backNode->next;
                    SSL_shutdown(backNode->ssl);
                    SSL_free(backNode->ssl);  
                    SSL_CTX_free(backNode->ctx);
                    close(backNode->controlFd);
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

//ͷ��һ���½ڵ�
/*NODE* createHeadNode(NODE* phead, int data) {
    NODE* newNode = initNode(data);
    NODE* ptemp = phead;
    if(phead){
        phead = newNode; //���½ڵ���Ϊͷ��
        newNode->next = ptemp;
    }else{
        return newNode;
    }
    return phead;
}

//��ǰ����
NODE* insertNodePre(NODE* phead,int key,int data) {
    NODE *newNode = initNode(data);
    if(phead){
        NODE* ptemp = phead;
        if(ptemp->data && ptemp->data == key){ //ֻ��һ���ڵ�
            phead = newNode;
            newNode->next = ptemp;
        }else{
            while(ptemp && ptemp->next){ 
                NODE* backNode = ptemp->next;
                if(backNode->data && backNode->data == key){ //�ҵ���һ���ڵ�
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

//������
NODE* insertNodeBack(NODE* phead,int key,int data) {
    NODE* newNode = initNode(data);
    if(phead){
        NODE* ptemp = phead;
        if(ptemp->data && ptemp->data == key){//ֻ��һ���ڵ�
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

//ͳ�ƽڵ����
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