#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>

int main(int argc, char** argv)
{
	if (argc <= 1) // because argc can be 0 in some cases
	{
		printf("Enter an argument.\n");
		return 0;
	}
	char *filename = argv[1]; // assuming argv[1] contains path
	FILE *fp = fopen(filename, "w+");
	int ruid = getuid();
	int euid = geteuid();
	fclose(fp);

	// getting the stat struct for the file
	struct stat st;

	stat(filename, &st);
	printf("%d\n", st.st_mode & S_IXOTH);

	return 0;
}