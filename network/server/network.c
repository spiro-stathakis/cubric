/*
1) Construct the local socket address.
2) Create the server socket.
3) Set the SO_REUSEADDR socket option.
4) Bind the local address to the server socket.
5) Listen for inbound connections.
6) Accept connections as they arrive.
*/
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include "defs.h"



int service_startup(); 
int handle_session(int session_id); 

/* define functions */

/* ************************************************************************* */

int handle_session(int session_fd) {
    

    struct sysinfo info;
    struct utsname kernel_info;
    int temp_length = 64  , return_length = 512;
    char *temp_string, *ret_buffer; 
    int cpu_count =  get_nprocs(); 
    
    /*
   	      struct sysinfo {
               long uptime;             Seconds since boot 
               unsigned long loads[3];  1, 5, and 15 minute load averages 
               unsigned long totalram;  Total usable main memory size 
               unsigned long freeram;   Available memory size 
               unsigned long sharedram; Amount of shared memory 
               unsigned long bufferram; Memory used by buffers
               unsigned long totalswap; Total swap space size 
               unsigned long freeswap;  swap space still available 
               unsigned short procs;    Number of current processes 
               char _f[22];             Pads structure to 64 bytes 
           };
   */
   
       	   
    temp_string = malloc(temp_length); 
    ret_buffer = malloc(return_length); 
    
    memset(temp_string,'\0', temp_length); 
    memset(ret_buffer,'\0',  return_length);
  
   
    
    
	
	 
	
    sysinfo(&info);
    uname(&kernel_info);
    
    snprintf(temp_string,sizeof(char *) * temp_length , "uptime:%lu\r\n" , info.uptime); 
    strcat(ret_buffer,temp_string); 
   
    snprintf(temp_string, sizeof(char *) * temp_length , "load 1:%lu\r\n" , info.loads[0]); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length , "load 5:%lu\r\n" , info.loads[1]); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "load 15:%lu\r\n" , info.loads[2]); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "totalram:%lu\r\n" , info.totalram); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "freeram:%lu\r\n" , info.freeram); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "sharedram:%lu\r\n" , info.sharedram); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "bufferram:%lu\r\n" , info.bufferram); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "totalswap:%lu\r\n" , info.totalswap); 
    strcat(ret_buffer,temp_string); 
    snprintf(temp_string, sizeof(char *) * temp_length  , "freeswap:%lu\r\n" , info.freeswap); 
    strcat(ret_buffer,temp_string); 
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "procs:%d\r\n" , info.procs); 
    strcat(ret_buffer,temp_string); 
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "cpus:%d\r\n" , cpu_count); 
    strcat(ret_buffer,temp_string); 
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "sysname:%s\r\n" , kernel_info.sysname); 
    strcat(ret_buffer,temp_string);
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "nodename:%s\r\n" , kernel_info.nodename); 
    strcat(ret_buffer,temp_string);
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "release:%s\r\n" , kernel_info.release); 
    strcat(ret_buffer,temp_string);
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "version:%s\r\n" , kernel_info.version); 
    strcat(ret_buffer,temp_string);
    
    snprintf(temp_string, sizeof(char *) * temp_length  , "machine:%s\r\n" , kernel_info.machine); 
    strcat(ret_buffer,temp_string);
    
    ssize_t count=write(session_fd,ret_buffer,strlen(ret_buffer));
    free(ret_buffer); 
    free(temp_string); 
    
    
    return count; 
    
    
}


int service_startup()
{ 

	
	char string_buffer[80]; 
	int err = 0; 
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM; /* allow tcp, exclude udp */ 
	hints.ai_protocol=0;
	hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
	struct addrinfo* res=0;
	
	
	err=getaddrinfo(ALLOWED_HOSTS,SERVER_PORT,&hints,&res);
	
	if (err != 0) 
	{
	    syslog(LOG_INFO, "Could not get getaddrinfo()");
            exit(EXIT_FAILURE); 
		
	} 
	/* need some error handling */ 
	
	
	/* socket to listen for connections */ 
	int server_fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	
	if (server_fd==-1)
	{
	    syslog(LOG_INFO, "Could not get socket()");
            exit(EXIT_FAILURE); 
		
	} 
	
	 
	
	/* use the reuse option */ 
	int reuseaddr=1;
	if (setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr)) == -1 )
	{
	    syslog(LOG_INFO, "Could not get setsockopt()");
            exit(EXIT_FAILURE); 
			
	} 
	 
	if (bind(server_fd,res->ai_addr,res->ai_addrlen) == -1)
	{
	    syslog(LOG_INFO, "Could not get bind()");
            exit(EXIT_FAILURE); 
		
	}
	
	if (listen(server_fd,SOMAXCONN) == -1 )
	{
	    syslog(LOG_INFO, "Could not get listen()");
            exit(EXIT_FAILURE); 
	}
	
		
	/* free up struct  used by getaddrinfo() */  
	freeaddrinfo(res);
	if (err > -1)
		return server_fd; 
	else 
	{
		
		return err; 
	
	}
} 

