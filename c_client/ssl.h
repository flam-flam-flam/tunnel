#include <openssl/ssl.h>
#include <openssl/err.h>
#define MAXBUF			1024
#define PRIKEY_PASSWD	"123456"						//prikey password
#define ALGO_TYPE		"RC4-MD5"						//algorithm type
#define SERVER_CERT		"certs/server/server.crt"		//server cert file
#define SERVER_KEYF		"certs/server/server.key"		//server key file
#define CLIENT_CERT		"certs/client/client.crt"		//client cert file
#define CLIENT_KEYF		"certs/client/client.key"		//client key file
#define ROOTCERTF		"certs/root/root.crt"			//root cert file


SSL_CTX* init_client_ctx(void);
int verify_callback(int ok, X509_STORE_CTX *store);
void load_certificates(SSL_CTX* ctx, char* CaFile, char* CertFile, char* KeyFile);
void show_certs_info(SSL* ssl);
ssize_t SSL_readn(SSL *fd, void *vptr, size_t n);
ssize_t SSL_writen(SSL *ssl, const void *vptr, size_t n);
SSL *ssl_create(int server);
void ssl_destroy(SSL *ssl, SSL_CTX *ctx);