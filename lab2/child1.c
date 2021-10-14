#include "unistd.h"
#include "stdio.h"
#include <string.h>
#include <ctype.h>

void toLower(char *str)
{
	int slen = strlen(str);
	for(int i = 0; i < slen; i++)
		str[i] = tolower(str[i]);
}

int main(int argc, char *argv[])
{
	char buffer[256];
	while(1)
	{
		fgets(buffer, 256, stdin);
		toLower(buffer);
		printf("%s", buffer);
		fflush(stdout);
	}
	return 0;
}

