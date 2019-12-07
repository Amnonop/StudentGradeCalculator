#include <stdlib.h>
#include <limits.h>
#include <stdio.h> 
#include <string.h> 
#include <errno.h>
#include <math.h>
#include <windows.h>
#include "dirent.h"

typedef enum {
	TM_SUCCESS,
	TM_FILE_OPEN_FAILED,
	TM_PROCESS_CREATE_FAILED,
	TM_PROCESS_WAIT_FAILED,
	TM_ERROR_CALCULATING_GRADE
} EXIT_CODES;

typedef int EXIT_CODE;

#define ID_LENGTH 9
#define NUM_OF_STUDENTS 10
#define STUDENT_DIR_LENGTH 17

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

EXIT_CODE runGradesCalculation(const char* grades_directory);
EXIT_CODE getStudentIdsFromFile(const char *grades_directory, const char *student_ids_filename, char *student_ids[], int *student_count);
EXIT_CODE createGradingProcess(const char *student_grades_directory, HANDLE process_handle[], int process_index);
void closeProcessHandles(HANDLE process_handles[], int process_count);

int main(int argc, char* argv)
{
	char* grades_directory = "C:\\katya\\StudentGradeCalculator\\ex02_testing\\input";//argv...
	char* final_grade_filename = "final_grades.txt";
	//MALLOC - no need for an array because it is in the stack?
	int students_grades[NUM_OF_STUDENTS] = { 0 };
	int students_ids[NUM_OF_STUDENTS] = { 0 };
	int file_handle_return_code = EXIT_SUCCESS;
	/*process for every student*/
	/*print their supid messages*/
	/*save grades in files*/
	char* curr_student_id = NULL;
	int student_dir_length = 0;
	
	EXIT_CODE exit_code = TM_SUCCESS;

	exit_code = runGradesCalculation(grades_directory);

	return 0;
}

EXIT_CODE runGradesCalculation(const char* grades_directory)
{
	char *student_ids_filename = "student_ids.txt";
	char *student_ids[NUM_OF_STUDENTS];
	int student_count = 0;
	EXIT_CODE exit_code = TM_SUCCESS;
	int process_count = 0;
	HANDLE process_handles[NUM_OF_STUDENTS];
	char *student_grades_directory;
	int student_grades_directory_length = 0;
	DWORD wait_result;
	int i = 0;
	DWORD process_exit_code;

	exit_code = getStudentIdsFromFile(grades_directory, student_ids_filename, student_ids, &student_count);
	if (exit_code != TM_SUCCESS)
		return exit_code;

	// Open a process to check the grade of each student
	for (process_count = 0; process_count < student_count; process_count++)
	{
		student_grades_directory_length = strlen(grades_directory) + 2 + STUDENT_DIR_LENGTH;
		student_grades_directory = (char*)malloc(sizeof(char)*student_grades_directory_length);
		sprintf_s(student_grades_directory, student_grades_directory_length, 
			"%s\\grades_%s", grades_directory, student_ids[process_count]);
		
		exit_code = createGradingProcess(student_grades_directory, process_handles, process_count);
		free(student_grades_directory);
		if (exit_code != TM_SUCCESS)
		{
			closeProcessHandles(process_handles, process_count);
			return exit_code;
		}
	}

	wait_result = WaitForMultipleObjects(student_count, process_handles, TRUE, INFINITE);
	if (wait_result == WAIT_FAILED)
	{
		closeProcessHandles(process_handles, process_count);
		return TM_PROCESS_WAIT_FAILED;
	}

	for (i = 0; i < process_count; i++)
	{
		GetExitCodeProcess(process_handles[i], &process_exit_code);
		if (process_exit_code != 0)
		{
			printf("Captain, we were unable to calculate %s", student_ids[i]);
			exit_code = TM_ERROR_CALCULATING_GRADE;
		}
		CloseHandle(process_handles[i]);
	}

	return exit_code;
}

EXIT_CODE getStudentIdsFromFile(const char *grades_directory, const char *student_ids_filename, char *student_ids[], int *student_count)
{
	char *filename;
	int filename_length = 0;
	FILE *file;
	errno_t exit_code;
	int i = 0;
	char student_id[ID_LENGTH + 1];

	filename_length = strlen(grades_directory) + 2 + strlen(student_ids_filename) + 1;
	filename = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(filename, filename_length, "%s\\%s", grades_directory, student_ids_filename);

	exit_code = fopen_s(&file, filename, "r");

	if (exit_code != 0)
	{
		printf("An error occured while openning file %s for reading.", student_ids_filename);
		return TM_FILE_OPEN_FAILED;
	}

	while (fgets(student_id, ID_LENGTH + 1, file) != NULL)
	{
		if (strcmp(student_id, "\n") == 0)
			continue;

		student_ids[i] = (char*)malloc(sizeof(char)*(ID_LENGTH + 1));
		strcpy_s(student_ids[i], ID_LENGTH + 1, student_id);
		i++;
	}
	*student_count = i;

	fclose(file);

	free(filename);

	return TM_SUCCESS;
}

EXIT_CODE createGradingProcess(const char *student_grades_directory, HANDLE process_handle[], int process_index)
{
	PROCESS_INFORMATION procinfo;
	BOOL is_process_created;
	CHAR process_name[] = "TestGrade.exe ";
	EXIT_CODE exit_code = TM_SUCCESS;

	int command_length = strlen(process_name) + strlen(student_grades_directory) + 2;
	char* command = (char*)malloc(sizeof(char)*command_length);
	sprintf_s(command, command_length, "TestGrade.exe %s", student_grades_directory);

	is_process_created = CreateProcessSimple(command, &procinfo);
	free(command);
	if (is_process_created == FALSE)
	{
		printf("Process Creation Failed!\n");
		return TM_PROCESS_CREATE_FAILED;
	}

	process_handle[process_index] = procinfo.hProcess;

	return exit_code;
}

void closeProcessHandles(HANDLE process_handles[], int process_count)
{
	int i = 0;
	for (i = 0; i < process_count; i++)
	{
		CloseHandle(process_handles[i]);
	}
}

int gradeSingleStudent(char* student_id)
{
	//char* log_filename = "Computation.txt";
	//char* solved_step;
	//char* solution_step = (char*)malloc(sizeof(char) * (strlen(student_id) + 1));
	//strcpy_s(solution_step, strlen(solution_step), student_id);
	//char* temp = "grade_";
	//strcat(temp, student_id);
	//strcpy(student_id, temp);
	CreateProcessSimpleMain(student_id);

	return EXIT_SUCCESS;
}

int CreateProcessSimpleMain(char* student_id)
{
	PROCESS_INFORMATION procinfo;
	DWORD waitcode;
	DWORD exitcode;
	BOOL retVal;
	CHAR process_name[] = "TestGrade.exe ";

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