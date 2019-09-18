/*
 *FileName  :   ssl_client.c
 *Author    :   JiangInk
 *Version   :   V0.1
 *Date      :   2015.03.10
*/

/*****************************************************************************/
/*** ssl_client.c                                                          ***/
/***                                                                       ***/
/*** Demonstrate an SSL client.                                            ***/
/*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "cJSON.h"

#ifndef _SSL_COMMON_H_
#define _SSL_COMMON_H_
#define FAIL			-1
#define MAXBUF			1024
#define PRIKEY_PASSWD	"123456"						//prikey password
#define ALGO_TYPE		"RC4-MD5"						//algorithm type
#define SERVER_CERT		"certs/server/server.crt"		//server cert file
#define SERVER_KEYF		"certs/server/server.key"		//server key file
#define CLIENT_CERT		"certs/client/client.crt"		//client cert file
#define CLIENT_KEYF		"certs/client/client.key"		//client key file
#define ROOTCERTF		"certs/root/root.crt"			//root cert file
#define SERVERHTML		\
 						"<html>\n" \
 							"<body>\n" \
 								"<h3>Client Request:</h3>\n" \
 								"<p>\n{\n%s\n}\n</p>\n" \
 								"<br/>\n" \
 								"<h3>Server Response:</h3>\n" \
								"<p>\n{\nHello! I am Server!\n}\n</p>\n" \
 							"</body>\n" \
 						"</html>"


#endif	/* !_SSL_COMMON_H_ */

/*---------------------------------------------------------------------*/
/*--- open_connection - create socket and connect to server.        ---*/
/*---------------------------------------------------------------------*/
int open_connection(const char *hostname, int port)
{
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if ( (host = gethostbyname(hostname)) == NULL )
    {
        perror(hostname);
        abort();
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}

/*---------------------------------------------------------------------*/
/*--- init_client_ctx - initialize the SSL engine.                  ---*/
/*---------------------------------------------------------------------*/
SSL_CTX* init_client_ctx(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;

    SSL_library_init();                 /* init algorithms library */
    OpenSSL_add_all_algorithms();       /* load & register all cryptos, etc. */
    SSL_load_error_strings();           /* load all error messages */
    //method = SSLv23_client_method();  /* create new server-method instance */
    method = TLSv1_client_method();
    ctx = SSL_CTX_new(method);          /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

/*---------------------------------------------------------------------*/
/*--- verify_callback - SSL_CTX_set_verify callback function.       ---*/
/*---------------------------------------------------------------------*/
int verify_callback(int ok, X509_STORE_CTX *store)
{
    char data[256];
    if (ok)
    {
        fprintf(stderr, "verify_callback\n{\n");
        X509 *cert = X509_STORE_CTX_get_current_cert(store);
        int  depth = X509_STORE_CTX_get_error_depth(store);
        int  err = X509_STORE_CTX_get_error(store);

        fprintf(stderr, "certificate at depth: %i\n", depth);
        memset(data, 0, sizeof(data));
        X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
        fprintf(stderr, "issuer = %s\n", data);
        X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
        fprintf(stderr, "subject = %s\n", data);
        fprintf(stderr, "error status:  %i:%s\n}\n", err, X509_verify_cert_error_string(err));
    }
    return ok;
}

/*---------------------------------------------------------------------*/
/*--- load_certificates - load from files.                          ---*/
/*---------------------------------------------------------------------*/
void load_certificates(SSL_CTX* ctx, char* CaFile, char* CertFile, char* KeyFile)
{
    #if 1
    /* set maximum depth for the certificate chain */
    //SSL_CTX_set_verify_depth(ctx, 1);
    /* set verify mode*/
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
    /* load CA certificate file */
    if (SSL_CTX_load_verify_locations(ctx, CaFile, NULL) <=0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    #endif
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set server private key password */ 
    SSL_CTX_set_default_passwd_cb_userdata(ctx, (void*)PRIKEY_PASSWD);
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
    /* set SSL cipher type */
    //SSL_CTX_set_cipher_list(ctx, ALGO_TYPE);
}

/*---------------------------------------------------------------------*/
/*--- show_certs_info - print out the certificates.                 ---*/
/*---------------------------------------------------------------------*/
void show_certs_info(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);   /* get the server's certificate */
    if ( cert != NULL )
    {
        //printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        //printf("Subject: %s\n", line);
        free(line);                         /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        //printf("Issuer: %s\n", line);
        free(line);                         /* free the malloc'ed string */
        X509_free(cert);                    /* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

/*---------------------------------------------------------------------*/
/*--- main - create SSL context and connect                         ---*/
/*---------------------------------------------------------------------*/
/*struct mType {
	int MsgType;
};
struct mLogin{
	struct mType type;
	char Token[32];
	int CSType;
};
struct mPack{
	int Type;
	struct mLogin msgLogin;
};*/
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


SSL_readn(SSL *fd, void *vptr, size_t n)
{
	ssize_t nleft;
	ssize_t nread;
	char *ptr;
	int quickack = 1;
	ptr = vptr;
	nleft = n;
	while (nleft > 0) 
	{
		if ((nread = SSL_read(fd, ptr, nleft)) < 0) 
		{
			if (errno == EINTR) 
			{
				nread = 0;  /* and call read() again */
			}
			else
			{
				return (-1);  /* exception occur */
			}
		}
		else if (nread == 0)
		{
			break;  /* EOF */
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);  /* return >= 0 */
}

int main(int count, char *strings[])
{
    SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char buf[1024];
    int bytes;
    char *hostname, *portnum;

    if ( count != 3 )
    {
        printf("usage: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }
	hostname = strings[1];
	portnum = strings[2];
	printf("<hostname>: %s <portnum> : %s\n", hostname, portnum);
    ctx = init_client_ctx();                                        /* initialize SSL */
    load_certificates(ctx, ROOTCERTF, CLIENT_CERT, CLIENT_KEYF);    /* load certs */
    server = open_connection(hostname, atoi(portnum));
    ssl = SSL_new(ctx);                 /* create new SSL connection state */
    SSL_set_fd(ssl, server);            /* attach the socket descriptor */
    if ( SSL_connect(ssl) == FAIL )     /* perform the connection */
    {
        ERR_print_errors_fp(stderr);
    }
    else
    {
		
		cJSON *mType, *mPack;
		cJSON *MsgLogin;
		mType=cJSON_CreateObject();
		MsgLogin=cJSON_CreateObject();
		mPack=cJSON_CreateObject();
		cJSON_AddNumberToObject(MsgLogin, "MsgType", 12);	
		//cJSON_AddItemToObject(MsgLogin, "mType", mType);
		cJSON_AddStringToObject(MsgLogin, "Token","mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw");
		cJSON_AddNumberToObject(MsgLogin, "CSType",4);
		cJSON_AddNumberToObject(mPack, "Type",12);
		cJSON_AddItemToObject(mPack, "Data", MsgLogin);
		printf("send mPack:%d ====== %s ===\n",cJSON_GetArraySize(mPack),cJSON_Print(mPack));

        char *msg = "{\"Type\":12,\"Data\":{\"MsgType\":12,\"Token\":\"mz4U4IO3q5E3O2HgcQyGj9YrHW1asBFw\",\"CSType\":4}}";
		
		//int length = strlen(msg);
		int length = strlen(cJSON_PrintUnformatted(mPack));
		struct message login1;
		struct message *pmsg = NULL;
		pmsg = &login1;
		(*pmsg).header = length;
		//strcpy((*pmsg).body, msg);
        strcpy((*pmsg).body, cJSON_PrintUnformatted(mPack));
        printf("struct: %d length:%d\n", strlen(cJSON_PrintUnformatted(mPack)),length);
        //printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        show_certs_info(ssl);                       /* get any certs */
		printf("send2 message: ====== %s ===\n",msg); 
        //SSL_write(ssl, pmsg, length+ 4);           /* encrypt & send message */
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
				//cJSON *item = cJSON_GetObjectItem(response, "Data"); 
				cJSON *item = cJSON_GetObjectItem(response,"Data");
				//item = cJSON_GetObjectItem(item,"Message");
				printf("item->valuestring :%s\n", cJSON_Print(item));
				cJSON_Delete(response);
			}
		}
        SSL_shutdown(ssl);                          /* shutdown SSL link */
        SSL_free(ssl);                             /* release connection state */
		//free(buflen);
		//cJSON_Delete(pack);
    }
    close(server);                                  /* close socket */
    SSL_CTX_free(ctx);                              /* release context */

    return 0;
}
