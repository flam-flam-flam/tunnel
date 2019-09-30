/*
 *FileName  :   at_client.c
 *Author    :   gaochuang
 *Version   :   V0.1
 *Date      :   2019.09.16
*/


#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>

#include "data.h"
#include "ssl.h"


int main(int count, char *strings[])
{
    SSL *ssl;
    
    char *hostname, *portnum;

    if ( count != 3 )
    {
        printf("usage: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }
	hostname = strings[1];
	portnum = strings[2];
	printf("<hostname>: %s <portnum> : %s\n", hostname, portnum);

    ssl = init_ssl_connetion(hostname, atoi(portnum));                /* create new SSL connection state */
    login(ssl);
    //close(server);                                  /* close socket */
    //SSL_CTX_free(ctx);                              /* release context */

    return 0;
}
