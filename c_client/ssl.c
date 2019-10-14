#include "ssl.h"


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
    method = SSLv23_client_method();  /* create new server-method instance */
    //method = TLSv1_client_method();
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


ssize_t SSL_readn(SSL *fd, void *vptr, size_t n)
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

ssize_t SSL_writen(SSL *ssl, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = SSL_write(ssl, ptr, nleft)) <= 0) 
        {
			if (nwritten < 0 && errno == EINTR) {
				nwritten = 0;  /* and call write() again */
			}
			else
				return (-1);
		}
		
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
