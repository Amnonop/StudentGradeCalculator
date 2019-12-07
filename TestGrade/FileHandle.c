#include <stdio.h>
#include "Commons.h"
#include "FileHandle.h"

int getGradeFromFile(char* filename)
{
	int sub_grade = 0;
	FILE *fp;
	errno_t error;
	error = fopen_s(&fp, filename, "r");

	if (error != 0)
		printf("An error occured while openning file %s for reading.", filename);

	else if (fp)
	{
		fscanf_s(fp, "%d", &sub_grade);
		fclose(fp);
	}

	return sub_grade;
}

EXIT_CODE readFromFile(char *filename, int *value)
{
	FILE *file;
	errno_t exit_code;

	exit_code = fopen_s(&file, filename, "r");

	if (exit_code != 0)
	{
		printf("An error occured while openning file %s for reading.", filename);
		return ERROR_OPEN_FILE;
	}

	fscanf_s(file, "%d", value);

	fclose(file);

	return 0;
}

/**
*	Writes the specified value to the file specified in filename.
*	
*	Accepts
*	-------
*	filename - a string representing the name of the file.
*	value - an integer containing the value that should be written into the file.
*
*	Returns
*	-------
*	An EXIT_CODE inidcating wether the write operation was succefull.
**/
EXIT_CODE writeToFile(char *filename, int value)
{
	FILE *file;
	errno_t exit_code;

	exit_code = fopen_s(&file, filename, "w");
	if (exit_code != 0)
	{
		printf("An error occured while openning file %s for writing.", filename);
		return ERROR_OPEN_FILE;
	}

	fprintf_s(file, "%d", value);

	fclose(file);

	return TG_SUCCESS;
}