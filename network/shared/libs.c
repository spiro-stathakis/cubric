
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./defs.h"


/* proto type */ 

char *read_file(char *file_path ); 


/* 
char *read_file(char *file_path )
accepts a file path as input and returns a char pointer to the string 
containing the file 
example: 

	char *contents = read_file("/proc/loadavg"); 
	printf("output is %s" , contents); 
*/ 


char *read_file(char *file_path )
{ 
	FILE *fp = fopen(file_path, "r");
	char c , *cp;
	int count =0; 
	cp = malloc(sizeof(char)); 
	while ((c  = fgetc(fp) ) != EOF)
	{	
		if (count > 0)
			cp = realloc(cp , (count + 1 ) * sizeof(char) ); 
		
		cp[count] = c;  
		count++; 
		
	}
	cp[count] = '\0'; 
	fclose(fp); 
	return cp; 	
} 
