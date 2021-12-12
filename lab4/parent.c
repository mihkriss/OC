#include "unistd.h"
#include "stdio.h"
#include <fcntl.h>
#include <string.h>
#include "mltshr.h"

int createChild(char *fname)
{
	switch(fork())
	{
		case -1:
		{
			return -1;
		}
		case 0:
		{
			char *args[] = {NULL};
			execv(fname, args);
			return -1;
		}
		default:
		{
			break;
		}
	}
	return 0;
}

int main()
{
	mltshr parent_child1;
	mltshr child1_child2;
	mltshr child2_parent;
	if(mltshr_create(&parent_child1, "parent_child1", 1) || mltshr_create(&child1_child2, "child1_child2", 1) || mltshr_create(&child2_parent, "child2_parent", 1))
	{
		printf("error: cannot create shared memory\n");
		return 1;
	}
	createChild("./child1");
	createChild("./child2");
	printf("Enter string:\n");
	char buffer[256];
	while(1)
	{
		fgets(buffer, 256, stdin);
		int slen = strlen(buffer);
		mltshr_write(&parent_child1, buffer, slen+1);
		char *input = mltshr_read(&child2_parent, &slen);
		printf("%s", input);
		free(input);
		fflush(stdout);
	}
	return 0;
}

