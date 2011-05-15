#include <stdio.h>
#include "nt.h"

int main(int argc,char *argv[])
{
	char result[66];
	char *p=strdup(argv[1]);
	
	SambaPassGen(p,result);
			
	/* printf("original password = %s\n", p); */
	printf("%s\n",result);

	return (0);
}
