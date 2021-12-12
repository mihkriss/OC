#include "unistd.h"
#include "stdio.h"
#include <string.h>
#include "mltshr.h"

void replaceSpaces(char *str)
{
	int slen = strlen(str);
	for(int i = 0; i < slen; i++)
		if(str[i] == ' ')
			str[i] = '_';
}

int main(int argc, char *argv[])
{
	mltshr child1_child2;
	mltshr child2_parent;
	if(mltshr_create(&child1_child2, "child1_child2", 0) || mltshr_create(&child2_parent, "child2_parent", 0))
	{
		printf("error: cannot connect to shared memory\n");
		return 1;
	}
	while(1)
	{
		int inputLen;
		char *input = mltshr_read(&child1_child2, &inputLen);
		replaceSpaces(input);
		mltshr_write(&child2_parent, input, strlen(input)+1);
		free(input);
	}
	return 0;
}

