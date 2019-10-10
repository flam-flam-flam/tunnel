#include "cJSON.h"
#include "ssl.h"
#include "ae.h"
#include "data.h"

#pragma pack(1)
struct atMessage {
	int32_t header;
	char body[512];
};
struct atData {
    int controlFd;
    SSL_CTX *ctx;
    SSL *ssl;
    aeEventLoop *loop;
    char buf[1024 * 8];
};
struct connectionData {
    NODE *connectionNode;
    aeEventLoop *loop;
}

struct atConnect {
    /* data */
    int MsgType;
    int TunnelID;
    int64_t ConnectinID;
    char LocalHost[16];
    int LocalPort;
    int Protocol;
};
#pragma pack()


void controlDataDestory(struct atData *data)
{
    SSL_shutdown(data->ssl);                          /* shutdown SSL link */
    SSL_free(data->ssl);  
    SSL_CTX_free(data->ctx);
    close(data->controlFd);
    free(data);
}

struct atData* controrlDataCreate(const char *hostname, int port)
{
    struct atData *data = NULL;
    int controlFd;
    controlFd = open_connection(hostname, port);
    if(controlFd < 0)
    {
        return NULL;     
    }
    data = malloc(sizeof(struct atData));
    if (data == NULL)
    {
        close(controlFd);
        return NULL;
    }
    memset(data, '\0', sizeof(struct atData));
    memset(data->buf, 0,sizeof(data->buf));
    data->ctx = init_client_ctx();
    data->controlFd = controlFd;
    printf("init controlFd = %d\n",controlFd);
    load_certificates(data->ctx, ROOTCERTF, CLIENT_CERT, CLIENT_KEYF);    /* load certs */
    data->ssl = SSL_new(data->ctx);                 /* create new SSL connection state */
    SSL_set_fd(data->ssl, controlFd);            /* attach the socket descriptor */
    if ( SSL_connect(data->ssl) == -1 )     /* perform the connection */
    {
        controlDataDestory(data);
        return NULL;
    }
    return data;
}
struct atConnect* connectDataCreate(cJSON *data)
{
    struct atConnect *clientConn = NULL;
    cJSON *jsonMsgType = cJSON_GetObjectItem(jsonData,"MsgType");
    cJSON *jsonTunnelID = cJSON_GetObjectItem(jsonData,"TunnelID");
    cJSON *jsonConnectinID = cJSON_GetObjectItem(jsonData,"ConnectinID");
    cJSON *jsonLocalHost = cJSON_GetObjectItem(jsonData,"LocalHost");
    cJSON *jsonLocalPort = cJSON_GetObjectItem(jsonData,"LocalPort");
    cJSON *jsonProtocol = cJSON_GetObjectItem(jsonData,"Protocol");
    clientConn->MsgType = jsonMsgType->valueint;
    clientConn->TunnelID = jsonTunnelID->valueint;
    clientConn->ConnectinID = jsonConnectinID->valueint;
    strcpy(clientConn->LocalHost,jsonLocalHost->valuestring);
    clientConn->LocalPort = jsonLocalPort->valueint;
    clientConn->Protocol = jsonProtocol->valueint;
    return clientConn;
}

void openConnection(void *userdata, cJSON *data)
{
    struct atConnect *clientConn = NULL;
    clientConn = connectDataCreate(data);
    connectcluster
}
void controlCommandHandler(aeEventLoop *el, int fd, void *userdata, int mask)
{
    struct atData *data = NULL;
    data = (struct atData *)userdata;
    struct atMessage message;

    printf("controlCommandHandler fd:%d\n",fd);
    if(AE_WRITABLE == mask)
    {    
        message.header = strlen(data->buf);
        strcpy(message.body,data->buf);
        
        SSL_write(data->ssl,&message, (message.header) + 4);
        aeDeleteFileEvent(el, fd, AE_WRITABLE);
    }
    else
    {
        int bytes;
        memset(&message,0,sizeof(message));
        bytes = SSL_readn(data->ssl, &(message.header), 4);       
        printf("Server msg1 bytes: %d message.header: %d \n",bytes, message.header);
        bytes = SSL_readn(data->ssl, message.body, message.header); 
        printf("Server msg1 bytes: %d message.body: %s \n",bytes, message.body);        
        if(message.body)
        {
            cJSON *response = cJSON_Parse(message.body);
            
            cJSON *jsonType = cJSON_GetObjectItem(response,"Type");
            cJSON *jsonData = cJSON_GetObjectItem(response,"Data");
            printf(" Data :%s, Type: %d\n", cJSON_Print(jsonData),jsonType->valueint);
            if(Type)
            {
                switch(jsonType->valueint)
                {
                    case 9://MSG_RESPONSE
                        cJSON *jsonStatus = cJSON_GetObjectItem(jsonData,"Status");
                        if(jsonStatus && jsonStatus->valueint == 10 )//STATUS_SUCCESS
                        {
                            printf("client login success");
                        }
                        else
                        {
                            printf("client login failed,please retry");
                        }
                        break;
                    case 5://MSG_CLIENT_OPEN_CONNECTION
                        //cJSON *jsonData = cJSON_GetObjectItem(response,"Data");
                        openConnection(void *userdata, cJSON *data);
                        printf("jsonMsgType:%d jsonTunnelID:%d jsonConnectinID:%ld jsonLocalHost:%s jsonLocalPort:%d jsonProtocol:%d\n",jsonMsgType->valueint,jsonTunnelID->valueint,jsonConnectinID->valueint,jsonLocalHost->valuestring, jsonLocalPort->valueint,jsonProtocol->valueint);
                        break;  
                    default:
                        break;
                }
            }
           // cJSON *ConnectinID = cJSON_GetObjectItem(Data,"ConnectinID");
            
            cJSON_Delete(response);
        }
    }
}

void connect_control_server(const char *hostname, int port)
{
    aeEventLoop *loop;
    struct atData *data = NULL;
    loop = aeCreateEventLoop(1024);

    data = controrlDataCreate(hostname,port);

    if(data == NULL)
    {
        return;
    }
    data->loop = loop;
    if (data->controlFd > 0 && aeCreateFileEvent(loop, data->controlFd ,AE_READABLE, controlCommandHandler,data) == AE_ERR) 
    {
        printf("aeCreateFileEvent failed\n");
    }

    printf("222222message222\n");
    //login(data->ssl);
    login2(data);
    aeMain(loop);
    aeDeleteEventLoop(loop);
}

void connect_data_server(void *userdata)
{
    
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

login2(struct atData *data)
{
    char *messageBody = NULL;

    messageBody = makeBody(12,"mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw", 4);
    strcpy(data->buf,messageBody);
   // printf("3333 message->body: %s message->length:%d\n", message.body,message.header);

    if (data->controlFd > 0 && aeCreateFileEvent(data->loop, data->controlFd ,AE_WRITABLE, controlCommandHandler,data) == AE_ERR) 
    {
        printf("aeCreateFileEvent failed2\n");
    }
}



login(SSL* ssl)
{
        int bytes;
        
		struct atMessage message;
        char *messageBody = NULL;

        messageBody = makeBody(12,"mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw",4);

        message.header = strlen(messageBody);
		//message = makeMessage(0,NULL,0);
        strcpy(message.body,messageBody);
	
        printf("222222message->body: %s message->length:%d\n", message.body,message.header);
        
        /*if (data->controlFd > 0 && aeCreateFileEvent(loop, data->controlFd ,AE_WRITABLE, controlCommandHandler,data) == AE_ERR) 
        {
            printf("aeCreateFileEvent failed2\n");
        }*/
        //show_certs_info(ssl);                       /* get any certs */
        SSL_write(ssl,&message, (message.header) + 4);   /* encrypt & send message */
        //aeDeleteFileEvent(loop, data->controlFd, AE_WRITABLE);

		/*while(1)
		{	
			struct atMessage message;
			memset(&message,0,sizeof(message));
			bytes = SSL_readn(ssl, &(message.header), 4); 
			
			printf("Server msg1 bytes: %d message.header: %d \n",bytes, message.header);
			bytes = SSL_readn(ssl, message.body, message.header); 
			printf("Server msg1 bytes: %d message.body: %s \n",bytes, message.body);
			
			if(message.body)
			{
				cJSON *response = cJSON_Parse(message.body);
				cJSON *item = cJSON_GetObjectItem(response,"Data");
				printf("item->valuestring :%s\n", cJSON_Print(item));
				cJSON_Delete(response);
			}
		}*/
}


