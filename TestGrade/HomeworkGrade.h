#ifndef HOMEWORK_GRADE_H
#define HOMEWORK_GRADE_H

#include <windows.h>
#include "Commons.h"

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
EXIT_CODE getHomeworkGrade(char* grades_directory, int hw_id, int hw_grades[], HANDLE hw_mutex_handle);

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
float calculateHomeworkGrade(int grades[]);

#endif
