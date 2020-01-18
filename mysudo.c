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
#define GROUP_COUNT_LIMIT 65536

// TODO [DOUBT] vulnerability: password check
// TODO [DOUBT] vulnerability: SIGKILL handler (to reset euid) (happens due to PCB?)
// TODO         vulnerability: prevent running the mysudo binary itself
// TODO         vulnerability: check if uid != euid initially
// TODO [DOUBT] feature: is mysudo required for running one's own files?
// TODO [DOUBT] feature: smarter search of commands?
// TODO [DOUBT] feature: need to pass UID too?
// TODO [DOUBT] size of input, input validation?
// TODO 		feature: thorough check of file permissions
// TODO         feature: handle cases for non-logable users

int main(int argc, char** argv)
{
	// checking argument count
	if (argc <= 2) // because argc can be 0 in some cases
	{
		printf("Please give input as follows:\n");
		printf("./mysudo [username-to-run-as] [path/to/command]\n");
		return 0;
	}

	// check existence of username
	char *username = argv[1]; // assuming argv[1] contains username
	struct passwd *passwd_entry = getpwnam(username);
	
	if (passwd_entry == NULL)
	{
		printf("No such user.\n");
		return 0;
	}

	int uid_requested = passwd_entry -> pw_uid;

	// checking existence of file
	char *file_path = argv[2]; // assuming argv[2] contains path

	int file_exists = access(file_path, F_OK) + 1; // 1 for exists and 0 otherwise
	if (!file_exists)
	{
		printf("Cannot find file_path.\n");
		return 0;	
	}

	// getting information about the caller
	int ruid_caller = getuid();
	int euid_caller = geteuid();

	// if every check (above) is passed
	seteuid(uid_requested);
	printf("UID: %d EUID: %d\n", getuid(), geteuid());
	int pid = fork();

	if (pid == 0) // child process
	{
		execvp(file_path, argv + 2);
	}
	else
	{
		wait(NULL); // understand the parameter NULL
		seteuid(ruid_caller);
		printf("UID: %d EUID: %d\n", getuid(), geteuid());
	}
	return 0;
}