#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>
#include <syslog.h>
#include <strings.h>
#include <string.h>
#include "defs.h"

/* declare prototypes */
void daemon_startup();
void daemon_shutdown(); 
void signal_handler(int signal); 
void signal_init();
 
 
/* define functions */

/* ************************************************************************* */
void daemon_startup()
{ 
	int pid, sid, i,pidFilehandle;
	char str[10];
        /* Fork*/
        pid = fork();
        if (pid < 0)
        {
            /* Could not fork */
            exit(EXIT_FAILURE);
        }

        if (pid > 0)
        {
            /* Child created ok, so exit parent process */
            printf("Child process created: %d\n", pid);
            exit(EXIT_SUCCESS);
        }

        /* Child continues */

        umask(027); /* Set file permissions 750 */
        /* Get a new process group */
        sid = setsid();
        if (sid < 0)
        {
            exit(EXIT_FAILURE);
        }

        /* close all descriptors */
        for (i = getdtablesize(); i >= 0; --i)
        {
            close(i);
        }

        /* Route I/O connections */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        chdir(RUN_DIR); /* change running directory */

        /* Ensure only one copy */
        pidFilehandle = open(PID_FILE, O_RDWR|O_CREAT, 0600);

        if (pidFilehandle == -1 )
        {
            /* Couldn't open lock file */
            syslog(LOG_INFO, "Could not open PID lock file %s, exiting", PID_FILE);
            exit(EXIT_FAILURE);
        }

        /* Try to lock file */
        if (lockf(pidFilehandle,F_TLOCK,0) == -1)
        {
            /* Couldn't get lock on lock file */
            syslog(LOG_INFO, "Could not lock PID lock file %s, exiting", PID_FILE);
            exit(EXIT_FAILURE);
        }

        /* Get and format PID */
        sprintf(str,"%d\n",getpid());

        /* write pid to lockfile */
        write(pidFilehandle, str, strlen(str));
	 
	
} 
/* ************************************************************************* */

/* ************************************************************************* */
void daemon_shutdown()
{
	closelog();  
	unlink(PID_FILE); 
	exit(EXIT_SUCCESS);
	 
} 
/* ************************************************************************* */
void signal_init()
{
	struct sigaction newSigAction; 
	sigset_t newSigSet; 
	
        /* Set signal mask - signals we want to block */
        sigemptyset(&newSigSet);
        sigaddset(&newSigSet, SIGCHLD);  /* ignore child - i.e. we don't need to wait for it */
        sigaddset(&newSigSet, SIGTSTP);  /* ignore Tty stop signals */
        sigaddset(&newSigSet, SIGTTOU);  /* ignore Tty background writes */
        sigaddset(&newSigSet, SIGTTIN);  /* ignore Tty background reads */
        sigprocmask(SIG_BLOCK, &newSigSet, NULL);   /* Block the above specified signals */

        /* Set up a signal handler */
        newSigAction.sa_handler = signal_handler;
        sigemptyset(&newSigAction.sa_mask);
        newSigAction.sa_flags = 0;

        /* Signals to handle */
        sigaction(SIGHUP, &newSigAction, NULL);     /* catch hangup signal */
        sigaction(SIGTERM, &newSigAction, NULL);    /* catch term signal */
        sigaction(SIGINT, &newSigAction, NULL);     /* catch interrupt signal */

	
	
	 
} 
/* ************************************************************************* */
void signal_handler(int signal)
{ 
	switch(signal)
        {
            case SIGHUP:
                syslog(LOG_WARNING, "Received SIGHUP signal.");
                break;
            case SIGINT:
            	syslog(LOG_INFO, "Received SIGINT signal so exiting");
                daemon_shutdown();
                break; 
            case SIGTERM:
                syslog(LOG_INFO, "Received SIGTERM signal so exiting");
                daemon_shutdown();
                break;
            default:
                syslog(LOG_WARNING, "Unhandled signal %s", strsignal(signal));
                break;
        }
         
	
} 
/* ************************************************************************* */
