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

//#include "data.h"
#include "ssl.h"
#include "zlog.h"
#include "config.h"


int main(int count, char *strings[])
{    
    char *hostname, *portnum;
    char *token;
    int rc; 
    struct config *cfg, *c;
    rc = dzlog_init("client.conf", "client");
    if (rc) 
    { 

        printf("init failed\n");
        return -1;

    }
    dzlog_info("hello, zlog. %s","iiiiiii am gaochuang");

    cfg = cfg_load_file("cfg_test.conf");
	if(!cfg){
		return 0;
	}
    dzlog_info("proxy.php.host = %s\n", cfg_getstr(cfg, "proxy.php.host"));
	dzlog_info("proxy.php.port = %d\n", cfg_getnum(cfg, "proxy.php.port"));
   
    c = cfg_get(cfg, "author");
	dzlog_info("author : %s\n", cfg_str(c));
	dzlog_info("url : %s\n", cfg_getstr(c, "url"));
	dzlog_info("\n");
    
    // zlog_profile();
    if ( count != 3 )
    {
        dzlog_info("usage: %s <hostname> <portnum>\n", strings[0]);
        hostname = cfg_getstr(cfg, "client.control.serverip");
        portnum = cfg_getstr(cfg, "client.control.serverport");
        
        if(!hostname || !portnum)
        {
            exit(0);
        }
        // exit(0);
    }
    else
    {
	    hostname = strings[1];
	    portnum = strings[2];
    }
    // token = cfg_getstr(cfg, "client.control.token");
	dzlog_info("<hostname>: %s <portnum> : %s\n", hostname, portnum);

    connect_control_server(hostname, atoi(portnum));  
    cfg_free(cfg);
    zlog_fini();
    return 0;
}
