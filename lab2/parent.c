#include "unistd.h"
#include "stdio.h"
#include <fcntl.h>
#include <string.h>

int createChild(char *fname, int read, int write)
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
			dup2(read, 0);
			dup2(write, 1);
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
	int pipe1[2];
	int pipe2[2];
	int pipe3[2];
	if (pipe(pipe1) == -1) { 
    printf("Pipe error!");
 	}
 	if (pipe(pipe2) == -1) {
    printf ("Pipe error!");
 	}
 	if (pipe(pipe3) == -1) {
    	printf("Pipe error!"); 
 	}
	if (createChild("./child1", pipe1[0], pipe2[1]))
	 	 printf("fork error"); 

	if (createChild("./child2", pipe2[0], pipe3[1]))
	    printf("fork error");
	printf("Enter string:\n");
	char buffer[256];
	while(1)
	{
		fgets(buffer, 256, stdin);
		int slen = strlen(buffer);
		write(pipe1[1], buffer, slen);
		read(pipe3[0], buffer, 256);
		printf("%s", buffer);
		fflush(stdout);
	}
	return 0;
}

