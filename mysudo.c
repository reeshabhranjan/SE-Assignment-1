#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pwd.h>
#include<grp.h>
#include<string.h>

// TODO vulnerability: password check
// TODO vulnerability: SIGKILL handler (to reset euid)

int main(int argc, char** argv)
{
	// checking argument count
	if (argc <= 1) // because argc can be 0 in some cases
	{
		printf("Enter an argument.\n");
		return 0;
	}

	// checking existence of file
	char *file_path = argv[1]; // assuming argv[1] contains path

	int file_exists = access(file_path, F_OK) + 1; // 1 for exists and 0 otherwise
	if (!file_exists)
	{
		printf("Cannot find file_path.\n");
		return 0;	
	}

	// getting information about the caller
	int ruid_caller = getuid();
	int euid_caller = geteuid();

	// getting the stat struct and info for the file
	struct stat st;
	stat(file_path, &st);
	int exec_user = st.st_mode & S_IXUSR;
	int uid_owner = st.st_uid;

	// checking if owner has the execute permission
	if (!exec_user)
	{
		printf("Permission denied. (owner doesn't have execute permission)\n");
		return 0;
	}
	else
	{
		// TODO ask for password
		seteuid(uid_owner);
		printf("UID: %d EUID: %d\n", getuid(), geteuid());
		int pid = fork();

		if (pid == 0) // child process
		{
			execvp(file_path, argv + 1);
		}
		else
		{
			wait(NULL); // understand the parameter NULL
			seteuid(ruid_caller);
			printf("UID: %d EUID: %d\n", getuid(), geteuid());
		}
	}

	return 0;
}