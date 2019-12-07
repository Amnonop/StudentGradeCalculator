#ifndef FILE_HANDLE_H
#define FILE_HANDLE_H

#include "Commons.h"

/* Reads a grade from the file specified in filename*/
int getGradeFromFile(char* filename);

EXIT_CODE readGradeFromFile(const char *grades_directory, const char *grade_filename, int *grade);

/*	Reads an integer from the file specified in filename. 
	Returns an integer representing an error code if an error occured,
	otherwise returns 0 and assigns the read integer to value. */
EXIT_CODE readFromFile(char *filename, int *value);

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
EXIT_CODE writeToFile(char *filename, int value);

#endif
