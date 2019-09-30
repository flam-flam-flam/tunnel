#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

/*---------------------------------------------------------------------*/
/*--- open_connection - create socket and connect to server.        ---*/
/*---------------------------------------------------------------------*/

int open_connection(const char *hostname, int port)
{
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if ( (host = gethostbyname(hostname)) == NULL)
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


/*
Read "n" bytes from a descriptor
*/
ssize_t readn(int fd, void *vptr, size_t n)
{
    ssize_t nleft;
    ssize_t nread;
    char *ptr;
    int quickack = 1;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) 
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            // read系统调用被中断，继续调用
            if (errno == EINTR) 
            {
                nread = 0;  /* and call read() again */
            }
            else
                return (-1);  /* exception occur */
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

ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
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