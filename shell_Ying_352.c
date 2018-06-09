#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define true 1
#define false 0
#define bool int
#define MAXLINE 80 /* The maximum length command */ 
const char space[2] = " ";
const char and[2] = "&";
const char esit[5] = "exit";

int tokenize_(char* str, char** tokens) 
{
    int num = 0;
    tokens[num] = strtok(str, space);
    while( tokens[num] != NULL ) 
    {
		num++;
		tokens[num] = strtok(NULL, space); // resume from the previous buffer " /n"
    }
    tokens[num] = NULL;
    return num;
}

int main(void)
{
	printf("CS149 Shell from Ying Wang\n");
	// fgets()
	// no need for fflush(stdout); 
	while (true) 
	{
		char buffer[MAXLINE]; // passed
        printf("Ying-352> ");
		fgets(buffer, MAXLINE, stdin);
		// get rid of '\n' or can be strtok(args, " \n")
        char *p = strchr(buffer, '\n');
        if(p)  
		{	
			*p = 0;
		}
		bool wait_on_child = false;
		bool is_exit = false;
		char *args[MAXLINE/2 + 1]; /* 41, command line with max 40 args + NULL */
        char **tokens;
		// tokenlize buffer to args
        int num_tokens = tokenize_(buffer, args);
        for(int i = 0; args[i] != NULL; i++) 
		{
            if (strcmp(args[i], esit) == 0) 
		    {
                is_exit = true;
            }
        }

        if (strcmp(args[num_tokens-1], and) == 0) 
        {
            wait_on_child = true;
            args[num_tokens-1] = NULL;
        }
		
		if(is_exit)
		{
			break;
		}
		if(num_tokens == 0)
		{
			continue;
		}
		// printf("Ying-352> %s\n", buffer); /* prompt- replace FirstName and L3SID */ buffer has been modified
        fflush(stdout); // will print the previous buffer from printf() to the console/disk
     	/* After reading user input, the steps are:*/
		// * (1) fork a child process using fork()
		// if child process after fork()
		int pid = -1;
		pid = fork();
		if(pid == -1)
		{
			// fork() failed 
			perror("fork");
			exit(1);
		}
		else if(pid == 0)
		{			
			// * (2) the child process will invoke execvp()
            execvp(args[0], args);
			perror("execvp");
			exit(1);
		}
		else if(wait_on_child == 0) 
		{
			wait(0);
			// wait(&status) eg error
			// 0 because shell line by line, not asked
			// real shell behavios doesn't care the process result
		}

		// parent don't care process, no wait()

	}
	return 0; 
}
