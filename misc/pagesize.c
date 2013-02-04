#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("Checking the system page size...\n");
	printf("Pagesize: %i\n", sysconf(_SC_PAGE_SIZE));
	return 0;
}
