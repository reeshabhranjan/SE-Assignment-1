#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pwd.h>
#include<grp.h>
#include<string.h>

int main(int argc, char** argv)
{
	// checking argument count
	if (argc <= 1) // because argc can be 0 in some cases
	{
		printf("Enter an argument.\n");
		return 0;
	}

	// checking existence of file
	char *command = argv[1]; // assuming argv[1] contains path

	int file_exists = access(command, F_OK) + 1; // 1 for exists and 0 otherwise
	if (!file_exists)
	{
		printf("Cannot find command.\n");
		return 0;
	}

	// getting information about the caller
	int ruid_caller = getuid();
	int euid_caller = geteuid();

	//*********debugging***********
	// printf("%d\n", euid_caller);
	// printf("args length: %d\n", (int)(sizeof(args) / sizeof(args[0])));
	// return 0;
	//*********debugging***********

	// getting the stat struct and info for the file
	struct stat st;
	stat(command, &st);
	int exec_user = st.st_mode & S_IXUSR;
	int exec_group = st.st_mode & S_IXGRP;
	int exec_other = st.st_mode & S_IXOTH;
	int setuid_bit = st.st_mode & S_ISUID;
	int setguid_bit = st.st_mode & S_ISGID;
	int uid_owner = st.st_uid;
	int gid_owner = st.st_gid;

	// checking file permissions
	// checking if owner has the execute permission
	if (!exec_user)
	{
		printf("Permission denied. (owner doesn't have execute permission)\n");
		return 0;
	}

	if (!setuid_bit)
	{
		printf("Permission denied. (file doesn't have a setuid_bit bit set)\n");
		return 0;
	}
	else
	{
		// TODO ask for password
		setuid(0);
		int pid = fork();

		if (pid == 0) // child process
		{
			// call execvp
			char args[argc - 2][100]; // for execvp command

			for (int i = 0; i < argc - 2; i++)
			{
				strcpy(args[i], argv[i + 2]);
			}
			
			execvp(command, args);
		}
		else
		{
			wait(NULL); // understand the parameter NULL
			setuid(ruid_caller);	
		}
	}

	return 0;
}