#ifndef TEST_GRADE_H
#define TEST_GRADE_H

/**
*	Calculated the total grade of the student and writes the final grade to a file.
*	Each grade is read separately from a file by a thread. When all threads are done, the final
*	grade is calculated and is written to a file.
*
*	Accepts
*	-------
*	grades_directory - a string representing the name of the directory containing the student's grades.
*
*	Returns
*	-------
*	An EXIT_CODE inidcating wether the operation was succefull.
**/
int calculateGrade(char* grades_directory);

#endif
