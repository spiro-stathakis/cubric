#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>

int main(int argc , char *argv[] )
{


	char *hostname = (argc >= 2) ? argv[1] : "localhost"; 
	
	char *portname = (argc == 3) ? argv[2] : "cubricd"; 
	/* 60000  by default*/ 
	
	
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=0;
	hints.ai_flags=AI_ADDRCONFIG;
	struct addrinfo* res=0;
	int err=getaddrinfo(hostname,portname,&hints,&res);
	if (err!=0) {
		fprintf(stderr,"Error: %s\n",strerror(errno));
		return errno; 
		
	}
	int fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (fd==-1) {
		fprintf(stderr,"Error: %s\n",strerror(errno));
		return errno; 
	}

	if (connect(fd,res->ai_addr,res->ai_addrlen)==-1) {
		fprintf(stderr,"Error: %s\n",strerror(errno));
		return errno; 
	}	
	
	
	freeaddrinfo(res);
	
	char buffer[256];
	for (;;) {
		ssize_t count=read(fd,buffer,sizeof(buffer));
		if (count<0) {
			if (errno!=EINTR)
			{
				fprintf(stderr,"Error: %s\n",strerror(errno));
				return errno; 
			}
		} else if (count==0) {
			break;
		} else {
			write(STDOUT_FILENO,buffer,count);
		}
	}
	close(fd);
	
	return 0; 
	

} 
