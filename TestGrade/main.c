#include <string.h> 
#include "Commons.h"
#include "TestGrade.h"

#define NUM_OF_ARGC 2
#define GRADE_DIRECTORY_ARG_INDEX 1

/**
*	Calculates the total grade of the student and writes the final grade to a file.
*	Each grade is read separately from a file by a thread. When all threads are done, the final
*	grade is calculated and is written to a file.
*
*	Accepts
*	-------
*	grades_directory - a string representing the name of the directory containing the student's grades.
**/
int main(int argc, char *argv[])
{
	int grades_directory_length = 0;
	char *grades_directory;
	EXIT_CODE exit_code;

	if (argc < NUM_OF_ARGC)
		return TG_NOT_ENOUGH_COMMAND_LINE_ARGUMENTS;

	grades_directory_length = strlen(argv[GRADE_DIRECTORY_ARG_INDEX]) + 1;
	grades_directory = (char*)malloc(sizeof(char)*grades_directory_length);
	strcpy_s(grades_directory, grades_directory_length, argv[GRADE_DIRECTORY_ARG_INDEX]);
	
	exit_code = calculateGrade(grades_directory);

	free(grades_directory);

	return exit_code;
}