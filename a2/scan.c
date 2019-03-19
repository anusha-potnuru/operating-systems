#include <stdio.h>
#include <stdlib.h>
int main()
{
	char c[1024];
	while(1)
	{
		// scanf("%[^\n]%*c",c);
		fgets(c, 1024, stdin);
		
		c[strlen(c)-1] = '\0';
		if(strlen(c) ==0)
			continue;
		printf("%d\n", strlen(c) );
		printf("%s\n",c );
		// c[0] =0;
	}

	return 0;
}