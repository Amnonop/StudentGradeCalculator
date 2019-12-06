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

ERROR_CODE readFromFile(char *filename, int *value)
{
	FILE *file;
	errno_t error_code;

	error_code = fopen_s(&file, filename, "r");

	if (error_code != 0)
	{
		printf("An error occured while openning file %s for reading.", filename);
		return ERROR_OPEN_FILE;
	}

	fscanf_s(file, "%d", &value);

	fclose(file);

	return 0;
}