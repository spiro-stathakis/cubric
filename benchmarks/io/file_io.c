#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/time.h>

/* continues like FALSE=0 , TRUE=1 , SOMETHING=2 etc...  */ 
enum{FALSE=0,TRUE}; 
#define MAX_FILE_LEN 128  
#define MAX_MSG_LEN 1024  

int shift_buffer(char *p_start , char *p_end);
int read_file( FILE *fptr, char *p_start , char *p_end); 
int write_file( FILE *fptr , char *p_start , char *p_end);


int main( int argc, char *argv[] )
{
 
   /*
   
   
   DESCRIPTION: 
   
   The aim of this program is to provide a simple mechanism to generate lots of io on the file system so that we 
   can monitor performance from the client.
   
   It program reads a file name as a command argument and it loads it char by char loads it into a buffer. 
   We then iterate this buffer and shift each char to the right. We write the resultant output to 
   a new file and then perform the process again with this new file. Continued enough times (256) we end up with 
   the file that we started with. 
   
   
   VARIABLES: 
   int: 
   i,j = variables for looping 
   size = size of the file being processed 
   
   char: 
   c = the char currently being read from the file 
   *p_start = the beginning of the buffer holding the file. 
   *p_end   = the end of the buffer holding the file. 
   *p_char  = move around the file 
   file_name[100]  =  the name of the file that is being written for iteration x 
   host_name[MAXHOSTNAMELEN] = the name of the current hosts (MAXHOSTNAMELEN is defined in sys/param.h)
   char logfile_name[MAXHOSTNAMELEN + 4]  = the name of the log file 
   
   
   FILE: 
   *fptr = the file that is currently being written 
   *logf_ptr = the logfile for this program 
   
   */ 
   
   int i,j , size =0 ; 
   
   char c, *p_start,*p_end,*p_char;
   char host_name[MAXHOSTNAMELEN];
   char log_file[MAXHOSTNAMELEN + 4];
   
   char input_file[MAX_FILE_LEN]; 
   char output_file[MAX_FILE_LEN]; 
   char msg[MAX_MSG_LEN]; 
   FILE *fptr,*logf_ptr;          

   
   struct timeval before , after;
	
   /* check startup params */ 
   if ( argc < 2 )
   {
      fprintf(stderr, "%s: No filename specified\n",argv[0]);
      return 1;
   }

   
   gethostname(host_name, sizeof(host_name)); 
   snprintf(log_file , sizeof(host_name) + 4 , "%s.log" , host_name); 
   snprintf(input_file , sizeof(input_file), "%s" , argv[1]); 
   
   
   /* open up log file */ 
   if ( ( logf_ptr = fopen( log_file, "w") ) == NULL )
   {
      fprintf(stderr, "%s: Cannot open for writing\n",log_file);
      return 2;   
   } 
   
   
   
   
   
   /* 
   	perform main body of io work 
   	on each iteration we read a file of size bytes and write a file of size byes. 
   	
   */
   
   
   for (i = 0 ;  i <= 1023 ; i++)
   {
   	  
   	   gettimeofday(&before , NULL);
   	   /* open a read file that was the previous write_file  if i > 0 */ 
   	   if (i > 0)
   	   	  snprintf(input_file,sizeof(input_file),"%s", output_file); 
   	  
	   
   	  /* open up a file for reading */ 
	   if ( ( fptr = fopen( input_file, "r") ) == NULL )
	   {
	   	   snprintf(msg, sizeof(msg) , "Error: Opening file for reading %s\n" , input_file); 
	   	   log_entry( msg , logf_ptr);
	   	   continue;
	   }
      	  
	   /* initialise  params */ 
	   size = file_size(fptr); 
	   p_start = (char *) malloc( size * sizeof(char)); 
	   p_end = p_char = p_start; 
	   for (j = 0 ; j < size ;  j++) p_end++; 
   
	   /* load template file into buffer and then close */ 
	   read_file(fptr, p_start , p_end);  
	   fclose(fptr); 
   	   
	   
	   /* shift the buffer */ 
	   shift_buffer(p_start,p_end);
   	   
	   /* write the output */ 
	   snprintf(output_file, sizeof(output_file) , "output-%s.%d",host_name , i);
   	   if ( ( fptr = fopen( output_file, "w") ) == NULL )
	   {
	   	   snprintf(msg, sizeof(msg) , "Error: Opening file for writing %s\n" , output_file); 
	   	   log_entry( msg , logf_ptr);
	   	   continue;
	   }
      	  
	   write_file(fptr, p_start, p_end); 
	   fclose(fptr); 

	   free(p_start); 
	   gettimeofday(&after , NULL);
	   snprintf(msg, sizeof(msg), "%d:%d:%d\n" , i , size, time_diff(before , after) );
	   log_entry(msg,logf_ptr);  
	   /* and do it again */ 
	   
   	   
   } /* end for loop */ 
    
   
   	   	   
   fclose(logf_ptr); 
    
}


/* ==================================================== */ 
int time_diff(struct timeval before , struct timeval after)
{
	
	return (long)after.tv_usec -  (long)before.tv_usec ;
	
} 
/* ==================================================== */ 
int log_entry(char *s , FILE *logf_ptr)
{
	fputs(s,logf_ptr); 		
	
} 
/* ==================================================== */ 
int shift_buffer(char *p_start , char *p_end)
{
	char *p = p_start; 
	unsigned char c; 
	while(p !=  p_end)
	{
	  c = *p; 
	  if ( c == 255)
	  	  c =0 ;
   	  else
   	  	  c++;
   	  *p = c; 
   	  p++; 
   	}
   	return TRUE; 
   	
} 
/* ==================================================== */ 
int print_buffer(char *p_start , char *p_end)
{
	
	char *p = p_start; 
	while(p !=  p_end)
	{
   	  printf("%c" , *p);
   	  p++; 
   	}
}
/* ==================================================== */ 
int write_file( FILE *fptr , char *p_start , char *p_end) 
{
	char *p = p_start; 
	while(p !=  p_end)
	{
	  fputc(*p , fptr); 
	  p++; 
   	}
   	return TRUE; 
} 

/* ==================================================== */ 
int read_file( FILE *fptr, char *p_start , char *p_end)
{

   char c , *p;  
   p = p_start; 
   while (   (c = fgetc(fptr)) != EOF)
   {
       if (p != p_end)
       {
       	       *p = c; 
       	       p++;
       }
   }	
   
   return TRUE; 
	
} 


/* ==================================================== */ 
int file_size(FILE *f)
{
	/* 
	seek to end of file
	get current file pointer
	seek back to beginning location
	*/ 
	int curr,end ;
	curr = ftell(f); 
	fseek(f, 0, SEEK_END);
	end = ftell(f); 
	fseek(f, curr, SEEK_SET); 
	return end;
}
