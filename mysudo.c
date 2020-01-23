
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

#define GROUP_COUNT_LIMIT 65536

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

void extract_commands(int argc, char** argv, char** command1, char** command2)
{
    int pipe_position = -1;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            pipe_position = i;
            break;
        }
    }

	// setting up command1 argument array

	if (pipe_position == -1)
	{
		command1 = NULL;
	}

	else
	{
		// TODO handle this if -u flag is provided
		command1 = (char **)(malloc(sizeof(char*) * (pipe_position - 1)));
		for (int i = 2; i < pipe_position - 1; i++)
		{
			command1[i - 2] = strdup(argv[i]);
		}
		command1[pipe_position - 2] = NULL;
	}

	// setting up command2 argument array

	if (pipe_position == -1)
	{
		command2 = argv;
	}

	else
	{
		command2 = (char **)(malloc(sizeof(char*) * (argc - pipe_position)));
		for (int i = pipe_position + 1; i < argc; i++)
		{
			command2[i - pipe_position - 1] = strdup(argv[i]);
		}
		command2[argc - pipe_position - 1] = NULL;
	}
}

int main(int argc, char** argv)
{
	signal(SIGINT, sigint_handler);
	// checking argument count
	if (argc <= 2) // because argc can be 0 in some cases
	{
		print_input_instructions();
		return 1;
	}

	// check existence of username
	char *username = argv[1]; // assuming argv[1] contains username
	struct passwd *passwd_entry = getpwnam(username);
	
	if (passwd_entry == NULL)
	{
		printf("No such user. Either you didn't provide a username or you provided non existing username.\n");
		print_input_instructions();
		return 1;
	}

	int uid_requested = passwd_entry -> pw_uid;

	// extracting commands if pipeline is present: ./mysudo reeshabh command1 | command2
	char** child_command; 
	char** parent_command;
	extract_commands(argc, argv, child_command, parent_command);

	int pipe_operation = child_command != NULL;

	// checking existence of file
	char *file_path_parent = parent_command[0]; // assuming argv[2] contains path
	char *file_path_child = child_command == NULL? NULL : child_command[0]; // assuming argv[2] contains path

	int file_exists_parent = access(file_path_parent, F_OK) + 1; // 1 for exists and 0 otherwise
	int file_exists_child = file_path_child != NULL && (access(file_path_child, F_OK) + 1); // 1 for exists and 0 otherwise
	
	
	if (!file_exists_parent)
	{
		printf("Cannot find file_path: \"%s\" Please check your input.\n", file_path_parent);
		print_input_instructions();
		return 1;	
	}

	if (pipe_operation)
	{
		if (!file_path_child)
		{
			printf("Cannot find file_path: \"%s\" Please check your input.\n", file_path_child);
			print_input_instructions();
			return 1;
		}
	}

	// manually check permissions for parent command if pipe-command
	if (pipe_operation)
	{
		// get information about file
		struct stat st;
		stat(file_path_parent, &st);
		int file_exec_permission_owner_parent = st.st_mode & S_IXUSR;
		int file_exec_permission_group_parent = st.st_mode & S_IXGRP;
		int file_exec_permission_other_parent = st.st_mode & S_IXOTH;
		int file_write_permission_owner_parent = st.st_mode & S_IWUSR;
		int file_write_permission_group_parent = st.st_mode & S_IWGRP;
		int file_write_permission_other_parent = st.st_mode & S_IWOTH;
		int file_group_id_parent = st.st_gid;
		int file_owner_uid_parent = st.st_uid;

		// get information about users
		int uid_terminal_user = getuid();
		int gid_terminal_user = getgid();
		int uid_sudo_user = uid_requested;
		int gid_sudo_user = getpwuid(uid_sudo_user) -> pw_gid;

		// obtain the group-membership of the requested user
		int group_count = GROUP_COUNT_LIMIT;
		gid_t groups[GROUP_COUNT_LIMIT];
		int gid_requested = gid_sudo_user;
		getgrouplist(username, gid_requested, groups, &group_count);
		int group_member = 0;

		for (int i = 0; i < group_count; i++)
		{
			if (file_group_id_parent == groups[i])
			{
				group_member = 1;
				break;
			}
		}

		int allowed = 0;

		// if running as owner but no execute permissions
		if (file_owner_uid_parent == uid_sudo_user)
		{
			if (!file_exec_permission_owner_parent || !file_write_permission_owner_parent)
			{
				printf("Requested user is the owner, but no execute permissions or no write permissions!\n");
				return 1;
			}
		}
		
		// if group member but no execute permissions
		else if (group_member)
		{
			if (!file_exec_permission_group_parent || !file_write_permission_group_parent)
			{
				printf("Requested user is in the group, but group has not execute permissions or no write permissions!\n");
				return 1;
			}
		}

		// if no others' execute permissions
		else if (!file_exec_permission_other_parent || !file_write_permission_other_parent)
		{
			printf("Other users do not have execute permissions or no write permissions!\n");
			return 1;
		}

		// check write permissions

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
		if (!pipe_operation)
		{
			execvp(parent_command[0], parent_command);
			perror(strcat(file_path_parent, "permission error"));
		}
		else
		{
			int fd[2];

			if (pipe(fd) == -1)
			{
				perror("Internal failure!");
				return 1;
			}

			int pid2 = fork();

			if (pid2 == 0)
			{
				close(1);
				dup(fd[1]);
				close(fd[0]);
				execvp(child_command[0], child_command);
			}

			else
			{
				wait(NULL);
				close(0);
				dup(fd[0]);
				close(fd[1]);
				execvp(parent_command[0], parent_command);
			}
		}
		
	}
	else
	{
		int status;
		wait(&status);
		seteuid(ruid_caller);
		print_restoring_euid();
		printf("The program: %s ended with return code: %d\n", file_path_parent, status);
	}
	return 0;
}