#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "remote.h"

#define PATH_MAX 4096
#define arg_count(arr) (int)(sizeof(&arr)/sizeof(char*))

// handle ctrl+c and ctrl+d
void sigHandler(int sig_num)
{
	signal(SIGINT, sigHandler);
	fflush(stdout);
}

int checkPipe(char * str){
	char *t;
	int i =0;
	for(t=str; *t != '\0'; t++){
		if(str[i] == '|'){
			return 1;
		}
		i++;
	}
	return 0;
}
void split(char* str, char** arr){
	for(int i =0; i < 50; i++){
		arr[i] = strsep(&str, " ");
		if(arr[i] == NULL){break;}
		if(strlen(arr[i]) == 0){i--;}

	}
}

	
void regularExc(char * str){
	// about command
	char* arr[50];
	split(str, arr);
	char* command = arr[0];
	if(strcmp(command, "about") == 0)
	{
		printf("SaiKiran \n"
			"2808586 \n"
			"CIS 340 MIS 1st semester \n"
			"Lab 1 \n");
	}

	// cwd command
	else if(strcmp(command, "cwd") == 0)
	{
		char cwd[100];
		printf("%s \n", getcwd(cwd, 100));
	}

	// cd command
	else if(strcmp(command, "cd") == 0)
	{
		char * dir = arr[1];
		printf("Directory: ");
		

		// no such directory
		if(chdir(dir) != 0)
		{
			perror("ERROR \n");
		}

		// after change
		else
		{
			char cwd[100];
			printf("Current working directory changed to %s \n", getcwd(cwd, 100));
		}
	}

	// run command
	else if(strcmp(command, "run") == 0)
	{
		char * com = arr[1];
		printf("Command: ");
		

		FILE *fp;
		char path[PATH_MAX];

		// use popen to read the command
		fp = popen(com, "r");

		// if there is no command
		if(fp == NULL)
		{
			perror("No command entered.\n");
			return;
		}

		// execution of the command
		while(fgets(path, PATH_MAX, fp) != NULL)
		{
			printf("%s", path);
		}

		// close it using pclose
		pclose(fp);
	}

	//the new commands
	else if(strcmp(command,"remote_echo")==0){
		if (arg_count(arr) < 3)
		{
			printf("%s\n %s\n", "Too few arguments", "remote_echo <server>  <text>");
		}
		char *server = arr[1];
		char *text = arr[2];
		remote_echo(server,text);
	}
	else if(strcmp(command,"remote_code")==0){
		if (arg_count(arr)<2){
			printf("%s\n %s\n", "Too few arguments", "remote_code <server>");
		}
		char *server = arr[1];
		char *name_id = "SaiKiran_2808586";
		remote_code(server,name_id);
	}

	else if(strcmp(command,"remote_msg")==0){
		if (arg_count(arr)<2){
			printf("%s\n %s\n", "Too few arguments", "remote_msg <server>");
		}
		char *server = arr[1];
		remote_msg(server);
	}
	



	// exit command
	else if(strcmp(command, "exit") == 0)
	{
		exit(0);
	}
}


int main()
{
	char command[10];

	// call function to handle ctrl+c and ctrl+d
	signal(SIGINT, sigHandler);

	while(1)
	{
		// read command every time
		//printf("\nSaikiran:~$ ");
		//scanf("%s", command);
		char* str = readline("Saikiran:~$ ");
		if(strlen(str) <= 0){
			continue;
		}else{
			add_history(str);
		}
		if(checkPipe(str)){
			char *str1 = strsep(&str, "|");
			char *str2 = strsep(&str, "|");
			char *arr1[50];
			split(str1, arr1);
			char *arr2[50];
			split(str2, arr2);
			pid_t p1;
			pid_t p2;
			int pipefd[2];
			p1 = fork();
			if(p1 == 0){
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
				if(execvp(arr1[0], arr1) < 0){
					printf("Error: Cannot execute command 1\n");
					exit(0);
				}
			}else{
				p2 = fork();
				if (p2 == 0){
					close(pipefd[1]);
					dup2(pipefd[0], STDIN_FILENO);
					close(pipefd[0]);
					if(execvp(arr2[0], arr2) < 0){
						printf("Error: Cannot execute command 2\n");
						exit(0);
					}
				}else {
					wait(NULL);
					wait(NULL);
				}
			}
		}else{
			regularExc(str);
		}
		
	}

	return 0;
}

/* REFERENCES
** 1. https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
** 2. https://man7.org/linux/man-pages/man3/getcwd.3.html
** 3. https://www.geeksforgeeks.org/write-a-c-program-that-doesnt-terminate-when-ctrlc-is-pressed/
*/