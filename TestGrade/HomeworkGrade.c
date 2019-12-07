#include <windows.h>
#include <stdbool.h>
#include "Commons.h"
#include "FileHandle.h"
#include "HomeworkGrade.h"

#define NUM_OF_CALC_HW 8

char* hw_file_names[NUM_OF_HW] =
	{	"ex01.txt","ex02.txt","ex03.txt",
		"ex04.txt", "ex05.txt", "ex06.txt",
		"ex07.txt", "ex08.txt", "ex09.txt", 
		"ex10.txt" 
	};

EXIT_CODE updateHWGrade(int hw_id, int hw_grade, int hw_grades[], HANDLE hw_mutex_handle);
void sortArray(int* hw_grades[NUM_OF_HW]);

EXIT_CODE getHomeworkGrade(char* grades_directory, int hw_id, int hw_grades[], HANDLE hw_mutex_handle)
{
	int hw_grade = 0;
	EXIT_CODE exit_code;

	exit_code = readGradeFromFile(grades_directory, hw_file_names[hw_id], &hw_grade);

	if (exit_code != 0)
		return exit_code;

	if (hw_grade < FAIL_THRESHOLD)
		hw_grade = 0;

	// This section should be locked
	exit_code = updateHWGrade(hw_id, hw_grade, hw_grades, hw_mutex_handle);
	if (exit_code != 0)
		return exit_code;

	return TG_SUCCESS;
}

EXIT_CODE updateHWGrade(int hw_id, int hw_grade, int hw_grades[], HANDLE hw_mutex_handle)
{
	DWORD wait_mutex_result;
	BOOL release_mutex_result;

	wait_mutex_result = WaitForSingleObject(hw_mutex_handle, INFINITE);
	if (wait_mutex_result != WAIT_OBJECT_0)
	{
		if (wait_mutex_result == WAIT_ABANDONED)
			return TG_MUTEX_ABANDONED;
		else
			return TG_MUTEX_WAIT_FAILED;
	}

	// Crtical section start
	hw_grades[hw_id] = hw_grade;

	release_mutex_result = ReleaseMutex(hw_mutex_handle);
	if (release_mutex_result == FALSE)
		return TG_MUTEX_RELEASE_FAILED;

	return TG_SUCCESS;
}

float calculateHomeworkGrade(int grades[])
{
	int i = 0;
	float hw_grade = 0.0;

	sortArray(grades);

	for (i = 0; i < NUM_OF_CALC_HW; i++)
	{
		hw_grade += grades[i];
	}

	hw_grade = (hw_grade / NUM_OF_CALC_HW);
	return hw_grade;
}

void sortArray(int* hw_grades[NUM_OF_HW])
{
	int i, j, a, number[NUM_OF_HW];

	for (i = 0; i < NUM_OF_HW; ++i)
	{

		for (j = i + 1; j < NUM_OF_HW; ++j)
		{

			if (hw_grades[i] < hw_grades[j])
			{
				a = hw_grades[i];
				hw_grades[i] = hw_grades[j];
				hw_grades[j] = a;
			}
		}
	}
}