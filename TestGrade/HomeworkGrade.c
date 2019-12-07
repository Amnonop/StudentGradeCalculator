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

/**
*	Reads the homework grade from the file and updates the grades array.
*
*	Accepts
*	-------
*	grades_directory - a string representing the name of the directory containing the file.
*   hw_id - an int representing the number of the homework.
*	hw_grades - an array containing all homework grades to be updated.
*	hw_mutex_handle - a handle to a mutex which locks the hw_grades array.
*
*	Returns
*	-------
*	An EXIT_CODE inidcating wether the operation was succefull.
**/
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

/**
*	Updates the homework grade array in the specified index with the homework grade.
*	The grades array is a shared resource so a mutex to lock it is used.
*
*	Accepts
*	-------
*   hw_id - an int representing the number of the homework to be updated.
*	hw_grade - and integer represening the grade.
*	hw_grades - an array containing all homework grades to be updated.
*	hw_mutex_handle - a handle to a mutex which locks the hw_grades array.
*
*	Returns
*	-------
*	An EXIT_CODE inidcating wether the operation was succefull.
**/
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

/**
*	Returns the average grade of the 8 best homework grades.
*
*	Accepts
*	-------
*	grades[] - an array containing the grades.
*
*	Returns
*	-------
*	A float containing the average grades of the homework.
**/
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

/**
*	Sorts an array from high to low.
*
*	Accepts
*	-------
*	hw_grades - the array to be sorted.
**/
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