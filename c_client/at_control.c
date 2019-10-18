#include <stdint.h>
#include "cJSON.h"
#include "ssl.h"
#include "ae.h"
#include "list.h"
#include <fcntl.h>
#include "adb_client.h"

#pragma pack(1)
struct atMessage {
	int32_t header;
	char body[512];
};
struct checkMessage {
    uint8_t type;
    uint64_t tunnleid;
    uint64_t connid;
};
struct atData {
    int Fd;
    SSL_CTX *ctx;
    SSL *ssl;
    aeEventLoop *loop;
    char buf[1024 * 8];
};
struct atClient {
    NODE *connectionNode;
    aeEventLoop *loop;
};

struct atTunnel {
    /* data */
    int MsgType;
    int TunnelID;
    int64_t ConnectinID;
    char LocalHost[16];
    int LocalPort;
    int Protocol;
};
struct dataChannel{
    int fd;
    SSL *ssl; 
    int TunnelID;
    int64_t ConnectinID;
};
#pragma pack()

void connectDataCreate(cJSON *jsonData,struct atTunnel *tunnelData)
{
    cJSON *jsonMsgType = cJSON_GetObjectItem(jsonData,"MsgType");
    cJSON *jsonTunnelID = cJSON_GetObjectItem(jsonData,"TunnelID");
    cJSON *jsonConnectinID = cJSON_GetObjectItem(jsonData,"ConnectinID");
    cJSON *jsonLocalHost = cJSON_GetObjectItem(jsonData,"LocalHost");
    cJSON *jsonLocalPort = cJSON_GetObjectItem(jsonData,"LocalPort");
    cJSON *jsonProtocol = cJSON_GetObjectItem(jsonData,"Protocol");
    tunnelData->MsgType = jsonMsgType->valueint;
    tunnelData->TunnelID = jsonTunnelID->valueint;
    tunnelData->ConnectinID = jsonConnectinID->valuedouble;
    strcpy(tunnelData->LocalHost,jsonLocalHost->valuestring);
    tunnelData->LocalPort = jsonLocalPort->valueint;
    tunnelData->Protocol = jsonProtocol->valueint;
    //return tunnelData;
}

void controlDataDestory(struct atClient *data)
{
    SSL_shutdown(data->connectionNode->ssl);                          /* shutdown SSL link */
    SSL_free(data->connectionNode->ssl);  
    SSL_CTX_free(data->connectionNode->ctx);
    close(data->connectionNode->Fd);
    free(data);
}

void clientChannelEventHandle(aeEventLoop *el, int fd, void *userdata, int mask)
{
    char buf[4096] = {'\0'};
    struct atClient *client = NULL;
    NODE *clientChannelNode = NULL;
    NODE *dataTunnelChannelNode = NULL;
    int bytes = 0;
    client= (struct atClient *)userdata;
    printf("clientChannelEventHandle receive data clientChannelEventHandle\n");
    bytes = read(fd, buf, 4096);
    if(!bytes)
    {
        printf("clientchannel maybe close\n");
        aeDeleteFileEvent(el, fd, AE_READABLE);
        //todo 释放资源
        return;
    }
    
    //bytes = readn(fd, buf, 4094);
    printf("clientChannelEventHandle receive data: [%s] bytes = %d\n",buf,bytes);
    if(client && client->connectionNode)
    {
        clientChannelNode = searchNode(client->connectionNode,fd);
        if (clientChannelNode)
        {
            dataTunnelChannelNode = searchAnotherNode(client->connectionNode,fd,clientChannelNode->clusterConnectinID);
            if(dataTunnelChannelNode && dataTunnelChannelNode->ssl)
            {
                SSL_write(dataTunnelChannelNode->ssl, buf, bytes);
                printf("send to cluster\n");
            }
        }
    }

}
void dataTunnelChannelEventHandle(aeEventLoop *el, int fd, void *userdata, int mask)
{
    if(mask == AE_READABLE)
    {
        char buf[4096] = {'\0'};
        struct atClient *client = NULL;
        NODE *clientChannelNode = NULL;
        NODE *dataTunnelChannelNode = NULL;
        int bytes = 0;

        client= (struct atClient *)userdata;

        printf("dataTunnelChannelEventHandle start\n");
        if(client && client->connectionNode)
        {
            dataTunnelChannelNode = searchNode(client->connectionNode,fd);
            
            if (dataTunnelChannelNode && dataTunnelChannelNode->ssl)
            {
                bytes = SSL_read(dataTunnelChannelNode->ssl, buf, 4096);  
                printf("dataTunnelChannelEventHandle bytes = %d errno :%d \n",bytes,errno);
                if(bytes > 0)
                {
                    clientChannelNode = searchAnotherNode(client->connectionNode,fd,dataTunnelChannelNode->clusterConnectinID);
                    if(clientChannelNode)
                    {
                        writen(clientChannelNode->Fd, buf, bytes);
                        //SSL_write(dataTunnelChannelNode->ssl, buf, 4096);
                        printf("send to client\n");
                    }
                } 
                else
                {
                    printf("dataTunnel maybe close\n");
                    aeDeleteFileEvent(el, fd, AE_READABLE);
                    //todo 释放资源
                }
                

            }
        }
    }
    else
    {
        int bytes = 0;
        struct atClient *client = NULL;
        struct checkMessage check;

        client = (struct atClient *)userdata;
        check.type = 1;//CS_CLIENT
        check.tunnleid = client->connectionNode->next->TunnelID;
        check.connid = client->connectionNode->next->clusterConnectinID;
        printf("tunnleid = %d connid = %d\n",client->connectionNode->next->TunnelID,client->connectionNode->next->clusterConnectinID);
        if(client->connectionNode->next->ssl)
        {
            printf("ch->ssl if true\n");
        }
        bytes = SSL_write(client->connectionNode->next->ssl,&check, sizeof(struct checkMessage));
        printf("8888888888 bytes = %d\n",bytes);
        aeDeleteFileEvent(el, fd, AE_WRITABLE);

    }

}
//channelType 0:controlTunnelChannel 1:dataTunnelChannel 2:clientChannel
NODE *connectServer(struct atClient *client, const char *hostname, int port, int channelType, struct atTunnel *tunnleData)
{
    int fd = 0;
    SSL_CTX *ctx;
    SSL *ssl;
    int flags;

    fd = open_connection(hostname, port);
    
    if(fd < 0)
    {
        printf("open connection failed\n");
        return client->connectionNode;     
    }
    // flags = fcntl(fd, F_GETFL, 0);
    // fcntl(fd, F_SETFL, flags | O_NONBLOCK);  
    printf("connectServer55555 channelType = %d fd = %d\n",channelType,fd);
    if(channelType == 2)
    {   
        if (client && client->loop && aeCreateFileEvent(client->loop, fd ,AE_READABLE, clientChannelEventHandle,client) == AE_ERR) 
        {
            printf("Add handle of clientChannelEvent failed\n");
        }
        printf("connectServer channelType = %d AE_READABLE\n",channelType);
        return createLastNode(client->connectionNode,fd,NULL,NULL,NULL,tunnleData->ConnectinID,tunnleData->TunnelID);
    }
    
    ctx = init_client_ctx();
    load_certificates(ctx, ROOTCERTF, CLIENT_CERT, CLIENT_KEYF);  
    ssl = SSL_new(ctx); 
    // SSL_set_verify(ssl,SSL_VERIFY_NONE,NULL);
    SSL_set_verify(ssl,SSL_VERIFY_NONE,NULL);
    SSL_set_fd(ssl, fd); 
    
    if ( SSL_connect(ssl) == -1 )
    {
        printf("ssl connect failed errno = %d\n",errno);
        SSL_shutdown(ssl);                          /* shutdown SSL link */
        SSL_free(ssl);  
        SSL_CTX_free(ctx);
        close(fd);

        return client->connectionNode;
    }
    if(channelType == 1)
    {
        int bytes = 0;
        struct checkMessage check;
        // struct dataChannel *ch;

        if (client && client->loop && aeCreateFileEvent(client->loop, fd ,AE_READABLE, dataTunnelChannelEventHandle,client) == AE_ERR) 
        {
            printf("Err, Add handle of dataTunnelChannelEven failed\n");
        }
        check.type = 1;//CS_CLIENT
        check.tunnleid = tunnleData->TunnelID;
        check.connid = tunnleData->ConnectinID;
        // ch->fd = fd;
        // ch->ssl = ssl;
        // ch->TunnelID = 11;//tunnleData->TunnelID;
        // ch->ConnectinID = 521645607827899;//tunnleData->ConnectinID;
        // if (aeCreateFileEvent(client->loop, fd ,AE_WRITABLE, dataTunnelChannelEventHandle,ch) == AE_ERR) 
        // {
        //     printf("aeCreateFileEvent dataCommandHandler failed2\n");
        // }
        // sleep(1);
        bytes = SSL_write(ssl,&check, 17);
        // if(ssl)
        // {
        //     printf("ssl is ture=====0000000\n");
        // }
        printf("99999999999999999 00bytes = %d\n",bytes);
        // while (1)
        // {
        //     /* code */
        // }
        
        return createLastNode(client->connectionNode,fd,ctx,ssl,NULL,tunnleData->ConnectinID,tunnleData->TunnelID); 
    }
    return createLastNode(client->connectionNode,fd,ctx,ssl,NULL,0,0);
    // return initNode(fd, ctx, ssl, NULL, 0, 0);
}
struct atClient* controrlDataCreate(const char *hostname, int port)
{
    struct atClient *client = NULL;
    client = malloc(sizeof(struct atClient));
    if (client == NULL)
    {
        // close(controlFd);
        return NULL;
    }
    memset(client, '\0', sizeof(struct atClient));
    client->connectionNode = NULL;
    
    if( !(client->connectionNode = connectServer(client,hostname,port, 0, NULL)))
    {
        controlDataDestory(client);
        return NULL;
    }

    return client;
}

void openConnection(void *userdata, cJSON *data)
{
    struct atTunnel tunnelData;
    struct atClient *client = NULL;
    client= (struct atClient *)userdata;
    connectDataCreate(data,&tunnelData);
    printf("tunnelData.ConnectinID :%ld \n",tunnelData.ConnectinID);
    // client->connectionNode = connectServer(client, "10.100.93.52", 37501, 1, &tunnelData);
    // if (client && client->loop && aeCreateFileEvent(client->loop, client->connectionNode->next->Fd ,AE_WRITABLE, dataTunnelChannelEventHandle,client) == AE_ERR) 
    // {
    //     printf("aeCreateFileEvent dataCommandHandler failed2\n");
    // }
    // client->connectionNode = connectServer(client, "10.100.106.79", 22223, 2, &tunnelData);
    client->connectionNode = connectServer(client, "10.1.18.11", 37501, 1, &tunnelData);
    // if (client && client->loop && aeCreateFileEvent(client->loop, client->connectionNode->next->Fd ,AE_WRITABLE, dataTunnelChannelEventHandle,client) == AE_ERR) 
    // {
    //     printf("aeCreateFileEvent dataCommandHandler failed2\n");
    // }
    // client->connectionNode = connectServer(client, "10.1.18.2", 22223, 2, &tunnelData);
    client->connectionNode = connectServer(client, "192.168.42.129", 5555, 2, &tunnelData);
    //connectcluster
}

void controlCommandHandler(aeEventLoop *el, int fd, void *userdata, int mask)
{
    struct atClient *data = NULL;
    data = (struct atClient *)userdata;
    struct atMessage message;

    printf("controlCommandHandler11 fd:%d mask=%d\n",fd,mask);
    if(AE_WRITABLE == mask)//2
    {    
        message.header = strlen(data->connectionNode->buf);
        strcpy(message.body,data->connectionNode->buf);
        printf("Server msg1 message.body: %s \n", data->connectionNode->buf);  
        SSL_write(data->connectionNode->ssl,&message, (message.header) + 4);
        aeDeleteFileEvent(el, fd, AE_WRITABLE);
    }
    else //AE_READABLE 1
    {
        int bytes;
        memset(&message,0,sizeof(message));
        bytes = SSL_readn(data->connectionNode->ssl, &(message.header), 4);
        if(!bytes)
        {
            aeDeleteFileEvent(el, fd, AE_READABLE);
            //controlDataDestory(data); //this is a bug
            //释放资源 Todo
            exit(0);//controltunnel close,exit application
        }  
        printf("Server msg1 bytes: %d message.header: %d errno = %d\n",bytes, message.header,errno);
        bytes = SSL_readn(data->connectionNode->ssl, message.body, message.header); 
        printf("Server msg1 bytes: %d message.body: %s errno = %d\n",bytes, message.body,errno);        
        if(message.body)
        {
            cJSON *response = cJSON_Parse(message.body);
            if(response)
            {
                cJSON *jsonType = cJSON_GetObjectItem(response,"Type");
                cJSON *jsonData = cJSON_GetObjectItem(response,"Data");
                
                printf(" userData :%s \n", (char *)userdata);
                if(jsonType)
                {
                    printf(" Data :%s, Type: %d\n", cJSON_Print(jsonData),jsonType->valueint);
                    switch(jsonType->valueint)
                    {
                        case 9://MSG_RESPONSE
                            {
                                cJSON *jsonStatus = cJSON_GetObjectItem(jsonData,"Status");
                                if(jsonStatus && jsonStatus->valueint == 10 )//STATUS_SUCCESS
                                {
                                    printf("client login success\n");
                                }
                                else
                                {
                                    printf("client login failed,please retry\n");
                                }
                                break;
                            }
                        case 5://MSG_CLIENT_OPEN_CONNECTION
                            openConnection(userdata,jsonData);

                            break;  
                        default:
                            printf("invalid event Type, unhandle\n");
                            break;
                    }
                }
                else
                {
                    printf("response format invalid\n");
                }
                cJSON_Delete(response);
            }
           
        }
    }
}

char *makeBody(int msgType, char *token, int csType)
{
    cJSON *mPack;
    cJSON *msgData;

	msgData=cJSON_CreateObject();
	mPack=cJSON_CreateObject();
	cJSON_AddNumberToObject(msgData, "MsgType", 12);	//MSG_TYPE_LOGIN
	cJSON_AddStringToObject(msgData, "Token","mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw");
	cJSON_AddNumberToObject(msgData, "CSType",4); //CSTYPE_CLIENT
	cJSON_AddNumberToObject(mPack, "Type",12);
	cJSON_AddItemToObject(mPack, "Data", msgData);
    return cJSON_PrintUnformatted(mPack);
}

void login2(struct atClient *data)
{
    char *messageBody = NULL;
    messageBody = makeBody(12,"mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw", 4);
    strcpy(data->connectionNode->buf,messageBody);
    printf("login2222\n");
    //AE_WRITABLE : 2  
    if (data->connectionNode && data->connectionNode->Fd > 0 && aeCreateFileEvent(data->loop, data->connectionNode->Fd ,AE_WRITABLE, controlCommandHandler,data) == AE_ERR) 
    {
        printf("aeCreateFileEvent failed2\n");
    }
}

void connect_control_server(const char *hostname, int port)
{
    aeEventLoop *loop;
    struct atClient *data = NULL;
    loop = aeCreateEventLoop(1024);

    data = controrlDataCreate(hostname,port);

    if(data == NULL)
    {
        return;
    }
    data->loop = loop;
    if (data->connectionNode && data->connectionNode->Fd > 0 && aeCreateFileEvent(loop, data->connectionNode->Fd ,AE_READABLE, controlCommandHandler,data) == AE_ERR) 
    {
        printf("aeCreateFileEvent failed\n");
    }

    //login(data->ssl);
    login2(data);
    aeMain(loop);
    aeDeleteEventLoop(loop);
}

void connect_data_server(void *userdata)
{
    
}


login(SSL* ssl)
{
    int bytes; 
    struct atMessage message;
    char *messageBody = NULL;
    messageBody = makeBody(12,"mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw",4);
    message.header = strlen(messageBody);
    strcpy(message.body,messageBody);
    printf("222222message->body: %s message->length:%d\n", message.body,message.header);
    SSL_write(ssl,&message, (message.header) + 4);
}


