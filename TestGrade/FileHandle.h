#ifndef FILE_HANDLE_H
#define FILE_HANDLE_H

/* Reads a grade from the file specified in filename*/
int getGradeFromFile(char* filename);

/*	Reads an integer from the file specified in filename. 
	Returns an integer representing an error code if an error occured,
	otherwise returns 0 and assigns the read integer to value. */
ERROR_CODE readFromFile(char *filename, int *value);

#endif
