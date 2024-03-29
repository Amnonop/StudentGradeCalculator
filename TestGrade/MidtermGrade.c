#include "MidtermGrade.h"
#include "Commons.h"
#include "FileHandle.h"

#define MIDTERM_FILENAME_LENGTH 11

/**
*	Reads the midterm grade from the file.
*
*	Accepts
*	-------
*	grades_directory - a string representing the name of the directory containing the file.
*
*	Returns
*	-------
*	An integer containing the grade of the midterm exam.
**/
int getMidtermGrade(const char* grades_directory)
{
	int midterm_grade;
	char* midterm_file_name = "midterm.txt";
	char* curr_file_path;
	int directory_path_length = strlen(grades_directory);
	int filename_length = directory_path_length + 2 + MIDTERM_FILENAME_LENGTH + 1;

	curr_file_path = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(curr_file_path, filename_length, "%s\\%s", grades_directory, midterm_file_name);
	midterm_grade = getGradeFromFile(curr_file_path);
	if (midterm_grade < FAIL_THRESHOLD)
		midterm_grade = 0;
	free(curr_file_path);
	return midterm_grade;
}