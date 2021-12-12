#include "unistd.h"
#include "stdio.h"
#include <string.h>
#include <ctype.h>
#include "mltshr.h"

void toLower(char *str)
{
	int slen = strlen(str);
	for(int i = 0; i < slen; i++)
		str[i] = tolower(str[i]);
}

int main(int argc, char *argv[])
{
	mltshr parent_child1;
	mltshr child1_child2;
	if(mltshr_create(&parent_child1, "parent_child1", 0) || mltshr_create(&child1_child2, "child1_child2", 0))
	{
		printf("error: cannot connect to shared memory\n");
		return 1;
	}
	while(1)
	{
		int inputLen;
		char *input = mltshr_read(&parent_child1, &inputLen);
		toLower(input);
		mltshr_write(&child1_child2, input, strlen(input)+1);
		free(input);
	}
	return 0;
}

