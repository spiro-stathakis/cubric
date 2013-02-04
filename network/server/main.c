 



/* network functions */ 
#include <time.h>
#include <stdarg.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h> 
#include "defs.h"

/* from daemon.c */ 
extern void daemon_startup();
extern void daemon_shutdown();
extern void signal_handler();
extern void signal_init();

/* from network.c */ 
extern int service_startup(); 
extern int handle_session(int session_fd);
 

extern *read_file(); 

/* main program */ 
int main(){ 
	
	
	
	
	setlogmask(LOG_UPTO(LOG_INFO));
        openlog(PROG_NAME, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
        syslog(LOG_INFO, "Program started");
        /* 
        	Check if parent process id is set not to 1 (init). 
        	If it is we are already a child and we can go straight to 
        	the task of waiting for a connection 
        */
        if (getppid() !=  1)
        {
        	signal_init();
        	daemon_startup();
        }

        /*
        	We are in the child process portion of the program. 
        	The parent process should not have made it to here. 
        */
        
        int server_fd = service_startup(); 
        
        syslog(LOG_INFO, "Daemon running");
         
        while (TRUE)
        {
            
            int session_fd=accept(server_fd,0,0);
            handle_session(session_fd); 
            close(session_fd);
            
           /* sleep(1); */ 
        }
	
	
	
} 


