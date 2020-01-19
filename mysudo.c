
/*
Author: Reeshabh Kumar Ranjan
Roll number: 2017086
Course: CSE-352: Security Engineering
*/

#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pwd.h>
#include<grp.h>
#include<string.h>
#include<shadow.h>
#include<grp.h>
#include<stdlib.h>
#include<limits.h>
#include<signal.h>

int ruid_caller;
int euid_caller;

void print_changing_euid()
{
	printf("Changing EUID...\n");
	printf("UID: %d EUID: %d\n", getuid(), geteuid());
	printf("=======================\n\n");
}

void print_restoring_euid()
{
	printf("\n=======================\n");
	printf("Restoring EUID...\n");
	printf("UID: %d EUID: %d\n", getuid(), geteuid());	
}

// common instructions
void print_input_instructions()
{
	printf("Please give input as follows:\n");
	printf("./mysudo [username_to_run_as] [path/to/command --with-arguments]\n");	
}

// to safely return on termination request
void sigint_handler(int signal_number)
{
	seteuid(euid_caller);
	print_restoring_euid();
	printf("\nTerminating program.\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	signal(SIGINT, sigint_handler);
	// checking argument count
	if (argc <= 2) // because argc can be 0 in some cases
	{
		print_input_instructions();
		return 0;
	}

	// check existence of username
	char *username = argv[1]; // assuming argv[1] contains username
	struct passwd *passwd_entry = getpwnam(username);
	
	if (passwd_entry == NULL)
	{
		printf("No such user. Either you didn't provide a username or you provided non existing username.\n");
		print_input_instructions();
		return 0;
	}

	int uid_requested = passwd_entry -> pw_uid;

	// checking existence of file
	char *file_path = argv[2]; // assuming argv[2] contains path

	int file_exists = access(file_path, F_OK) + 1; // 1 for exists and 0 otherwise
	if (!file_exists)
	{
		printf("Cannot find file_path. Please check your input.\n");
		print_input_instructions();
		return 0;	
	}

	// getting information about the caller
	ruid_caller = getuid();
	euid_caller = geteuid();

	// if every check (above) is passed
	seteuid(uid_requested);
	print_changing_euid();
	int pid = fork();

	if (pid == 0) // child process
	{
		execvp(file_path, argv + 2);
		perror(strcat(file_path, "permission error"));
	}
	else
	{
		int status;
		wait(&status);
		seteuid(ruid_caller);
		print_restoring_euid();
		printf("The program: %s ended with return code: %d\n", file_path, status);
	}
	return 0;
}