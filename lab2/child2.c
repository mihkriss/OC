#include "unistd.h"
#include "stdio.h"
#include <string.h>

void replaceSpaces(char *str)
{
	int slen = strlen(str);
	for(int i = 0; i < slen; i++)
		if(str[i] == ' ')
			str[i] = '_';
}

int main(int argc, char *argv[])
{
	char buffer[256];
	while(1)
	{
		fgets(buffer, 256, stdin);
		replaceSpaces(buffer);
		printf("%s", buffer);
		fflush(stdout);
	}
	return 0;
}

