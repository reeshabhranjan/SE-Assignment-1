#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<pwd.h>
#include<grp.h>

int main(int argc, char** argv)
{
	if (argc <= 1) // because argc can be 0 in some cases
	{
		printf("Enter an argument.\n");
		return 0;
	}

	char *filename = argv[1]; // assuming argv[1] contains path

	int file_exists = access(filename, F_OK) + 1; // 1 for exists and 0 otherwise

	if (!file_exists)
	{
		printf("Cannot find command.\n");
		return 0;
	}

	FILE *fp = fopen(filename, "w+");
	int ruid_caller = getuid();
	int euid_caller = geteuid();
	fclose(fp);

	// getting the stat struct for the file
	struct stat st;
	stat(filename, &st);
	int exec_user = st.st_mode & S_IXUSR;
	int exec_group = st.st_mode & S_IXGRP;
	int exec_other = st.st_mode & S_IXOTH;
	int uid_owner = st.st_uid;
	int gid_owner = st.st_gid;

	printf("uid_caller: %d\n", ruid_caller);
	printf("uid_owner:  %d\n", uid_owner);

	return 0;
}