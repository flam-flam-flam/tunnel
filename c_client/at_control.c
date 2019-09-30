#include "data.h"

#include "cJSON.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma pack(1)
struct message{
	int32_t header;
	char body[512];
};
#pragma pack()

struct messageLen{
	int32_t len;
};
struct messageBody{
	char body[512];
};

login(SSL* ssl)
{
        int bytes;
    	cJSON *mType, *mPack;
		cJSON *MsgLogin;
		mType=cJSON_CreateObject();
		MsgLogin=cJSON_CreateObject();
		mPack=cJSON_CreateObject();
		cJSON_AddNumberToObject(MsgLogin, "MsgType", 12);	
		cJSON_AddStringToObject(MsgLogin, "Token","mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw");
		cJSON_AddNumberToObject(MsgLogin, "CSType",4);
		cJSON_AddNumberToObject(mPack, "Type",12);
		cJSON_AddItemToObject(mPack, "Data", MsgLogin);
		printf("send mPack:%d ====== %s ===\n",cJSON_GetArraySize(mPack),cJSON_Print(mPack));

        char *msg = "{\"Type\":12,\"Data\":{\"MsgType\":12,\"Token\":\"mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw\",\"CSType\":4}}";
		int length = strlen(cJSON_PrintUnformatted(mPack));
		struct message login1;
		struct message *pmsg = NULL;
		pmsg = &login1;
		(*pmsg).header = length;
        strcpy((*pmsg).body, cJSON_PrintUnformatted(mPack));
        printf("struct: %d length:%d\n", strlen(cJSON_PrintUnformatted(mPack)),length);
        show_certs_info(ssl);                       /* get any certs */
		printf("send2 message: ====== %s ===\n",msg); 
        SSL_write(ssl, pmsg, length + 4);           /* encrypt & send message */
		while(1)
		{	
			struct message mresp;
			memset(&mresp,0,sizeof(mresp));
			bytes = SSL_readn(ssl, &(mresp.header), 4); 
			
			printf("Server msg1 bytes: %d mresp.header: %d \n",bytes, mresp.header);
			bytes = SSL_readn(ssl, mresp.body, mresp.header); 
			printf("Server msg1 bytes: %d mresp.body: %s \n",bytes, mresp.body);
			
			if(mresp.body)
			{
				
				cJSON *response = cJSON_Parse(mresp.body);
				cJSON *item = cJSON_GetObjectItem(response,"Data");
				printf("item->valuestring :%s\n", cJSON_Print(item));
				cJSON_Delete(response);
			}
		}
}