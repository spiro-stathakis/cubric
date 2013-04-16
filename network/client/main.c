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
	struct timeval timeout;      
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	int err=getaddrinfo(hostname,portname,&hints,&res);

	if (err != 0) {
		fprintf(stderr,"Error: %s\n",gai_strerror(err));
		return err; 
	}

	int sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	
	if (sockfd==-1) {
		fprintf(stderr,"Socket error: %s\n",strerror(errno));
		return errno; 
	}

	setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)); 
	
	if (connect(sockfd,res->ai_addr,res->ai_addrlen)==-1) {
		fprintf(stderr,"Socket connect error: %s\n",strerror(errno));
		return errno; 
	}	
	
	freeaddrinfo(res);
	char buffer[512];
	for (;;) {
		ssize_t count=read(sockfd,buffer,sizeof(buffer));
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
	close(sockfd);
	return 0; 
} 
