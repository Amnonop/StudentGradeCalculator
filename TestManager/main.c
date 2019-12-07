#include <stdlib.h>
#include <limits.h>
#include <stdio.h> 
#include <string.h> 
#include <errno.h>
#include <math.h>
#include <windows.h>
//#include "dirent.h"



#define FAIL_THRESHOLD 60
#define NUM_OF_HW 10
#define HW_FILENAME_LENGTH 9
#define NUM_OF_CALC_HW 8
#define MIDTERM_FILENAME_LENGTH 11
#define EXAM_FILENAME_LENGTH 9
#define NUM_THREADS 13
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define NUM_OF_STUDENTS 10
#define EXIT_FILE_HANDLE_ERROR 100
#define EXIT_PROCESS_HANDLE_ERROR -100
#define EXIT_SON_PROCESS_CALCULATION_ERROR 200
#define MAX_EXPRESSION_LENGTH 256
#define PROCESS_HANDLE_H
#define TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55

/*global variables*/
static int midterm_grade = 0;
static int exam_grade = 0;




/*declerations*/
int openGradeFile();
int printGradeFile(int id, int grade);
int CreateProcessSimpleMain(char* student_id);
BOOL CreateProcessSimple(LPTSTR command_line, PROCESS_INFORMATION *process_info_ptr);
int gradeSingleStudent(char* student_id);

int main()
{
	char* grades_directory = "grades_123456789";//argv...
	char* final_grade_filename = "final_grades.txt";
	//MALLOC - no need for an array because it is in the stack?
	int students_grades[NUM_OF_STUDENTS] = { 0 };
	int students_ids[NUM_OF_STUDENTS] = { 0 };
	int file_handle_return_code = EXIT_SUCCESS;
	/*process for every student*/
	/*print their supid messages*/
	/*save grades in files*/
	char* curr_student_id = NULL;
	/*while ID in DIR*/
	gradeSingleStudent(curr_student_id);

	file_handle_return_code = openGradeFile(final_grade_filename);
	if (file_handle_return_code != EXIT_SUCCESS)
		return file_handle_return_code;
	for (int i = 0; i < NUM_OF_HW; i++)
	{
		if (students_ids[i] != 0)
		{	/*write to grades file "ID_GRADE"*/
			file_handle_return_code = printGradeFile(students_ids[i], students_grades[i]);
			if (file_handle_return_code != EXIT_SUCCESS)
				return file_handle_return_code;
		}
	}
	return 0;
}


int gradeSingleStudent(char* student_id)
{
	char* log_filename = "Computation.txt";
	char* solved_step;
	char* solution_step = (char*)malloc(sizeof(char) * (strlen(student_id) + 1));
	strcpy_s(solution_step, strlen(solution_step), student_id);

	CreateProcessSimpleMain(student_id);

	return EXIT_SUCCESS;
}

int CreateProcessSimpleMain(char* student_id)
{
	PROCESS_INFORMATION procinfo;
	DWORD waitcode;
	DWORD exitcode;
	BOOL retVal;
	CHAR process_name[] = ("TestGrade.exe ");

	int command_length = strlen(student_id) + strlen(process_name) + 2;
	char* command = (char*)malloc(sizeof(char)*command_length);
	sprintf_s(command, command_length, "TestGrade.exe %s", student_id);

	retVal = CreateProcessSimple(command, &procinfo);

	if (retVal == 0)
	{
		printf("Process Creation Failed!\n");
		return EXIT_PROCESS_HANDLE_ERROR;
	}

	waitcode = WaitForSingleObject(
		procinfo.hProcess,
		TIMEOUT_IN_MILLISECONDS);

	if (waitcode == WAIT_TIMEOUT)
	{
		printf("Process was not terminated before timeout!\n"
			"Terminating brutally!\n");
		TerminateProcess(
			procinfo.hProcess,
			BRUTAL_TERMINATION_CODE);
		Sleep(10);
		return EXIT_PROCESS_HANDLE_ERROR;
	}

	GetExitCodeProcess(procinfo.hProcess, &exitcode);

	CloseHandle(procinfo.hProcess);
	CloseHandle(procinfo.hThread);

	return exitcode;
}


BOOL CreateProcessSimple(LPTSTR command_line, PROCESS_INFORMATION *process_info_ptr)
{
	STARTUPINFO	startinfo = { sizeof(STARTUPINFO), NULL, 0 };

	return CreateProcess(NULL,
		command_line,
		NULL,
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&startinfo,
		process_info_ptr
	);
}


int openGradeFile()
{
	/*opens final grades file*/
	char* filename = "final_grades.txt";
	FILE *fp;
	errno_t error;
	error = fopen_s(&fp, filename, "w");
	if (error != 0)
	{	
		printf("An error occured while openning file %s for writing.", filename);
		return EXIT_FILE_HANDLE_ERROR;
	}
	if (fp)
		fclose(fp);

	return EXIT_SUCCESS;
}

int printGradeFile(int id, int grade)
{
	/*appends grades to final grades file*/
	FILE *fp;
	errno_t error;

	error = fopen_s(&fp, "final_grades.txt", "a");
	if (error != 0)
	{
		printf("An error occured while openning file %s for writing final_grades.txt");
		return EXIT_FILE_HANDLE_ERROR;
	}

	else if (fp)
	{
		fprintf_s(fp, "%d %d\n", id, grade);
		fclose(fp);
	}
	return EXIT_SUCCESS;
}

/*
void listFilesRecursively(char *basePath)
{
	char path[1000];
	struct dirent *dp;
	DIR *dir = opendir(basePath);

	// Unable to open directory stream
	if (!dir)
		return;

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			printf("%s\n", dp->d_name);

			// Construct new path from our base path
			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			listFilesRecursively(path);
		}
	}

	closedir(dir);
}*/