#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>

int main(int argc, char** argv)
{
	// checking argument count
	if (argc <= 1) // because argc can be 0 in some cases
	{
		printf("Enter an argument.\n");
		return 0;
	}

	// checking existence of file
	char *filename = argv[1]; // assuming argv[1] contains path
	int file_exists = access(filename, F_OK) + 1; // 1 for exists and 0 otherwise
	if (!file_exists)
	{
		printf("Cannot find command.\n");
		return 0;
	}

	// getting information about the caller
	int ruid_caller = getuid();
	int euid_caller = geteuid();

	printf("%d\n", euid_caller);

	// getting the stat struct and info for the file
	struct stat st;
	stat(filename, &st);
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
		

	}

	return 0;
}