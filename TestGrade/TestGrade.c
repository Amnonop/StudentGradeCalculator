#include <stdlib.h>
#include <limits.h>
#include <stdio.h> 
#include <string.h> 
#include <errno.h>
#include <math.h>
#include <windows.h>
#include <stdbool.h>
#include <tchar.h>
#include "TestGrade.h"
#include "Commons.h"
#include "FileHandle.h"
#include "MidtermGrade.h"
#include "ThreadHandle.h"
#include "HomeworkGrade.h"

#define HW_FILENAME_LENGTH 9
#define EXAM_FILENAME_LENGTH 9
#define NUM_THREADS 12
#define BRUTAL_TERMINATION_CODE 0x55
#define ID_LENGTH 9
#define FINAL_GRADE_FILENAME_LENGTH 20

/*declerations*/
int getExamGrade(char* grades_directory);
int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade);
DWORD WINAPI midtermGradeThread(LPVOID lpParam);
DWORD WINAPI getExamGradeThread(LPVOID lpParam);

DWORD WINAPI hwGradeThread(LPVOID lpParam);

EXIT_CODE createHWMutex(HANDLE mutex_handle);
HANDLE createMutexSimple(LPCTSTR mutex_name);

EXIT_CODE writeFinalGradeToFile(const char *grades_directory, int final_grade);
BOOL closeThreadHandles(HANDLE thread_handles[], int thread_count);

typedef struct _hw_thread_params
{
	char *grades_directory;
	int hw_id;
	int *hw_grades;
	HANDLE hw_mutex_handle;
} hw_thread_params;

int calculateGrade(char* grades_directory)
{
	/*10 HW*/
	/*MIDTERM*/
	/*FINAL A*/
	/*FINAL B*/
	char hw_mutex_name[] = "hw_mutex";
	HANDLE hw_mutex_handle;
	
	HANDLE thread_handles[NUM_THREADS];
	DWORD thread_ids[NUM_THREADS];
	int thread_count = 0;
	
	int hw_grades[NUM_OF_HW] = { 0 };
	hw_thread_params thread_params[NUM_OF_HW];
	DWORD wait_code;
	BOOL ret_val;
	EXIT_CODE exit_code;
	int i = 0;
	int hw_id = 0;
	float hw_grade = 0.0;
	int final_grade = 0;
	BOOL has_errors = FALSE;

	// Create mutex
	hw_mutex_handle = createMutexSimple(hw_mutex_name);
	if (hw_mutex_handle == NULL)
	{
		printf("Error creating HW mutex: %d\n", GetLastError());
		CloseHandle(hw_mutex_handle);
		return TG_MUTEX_CREATE_FAILED;
	}

	// Create two threads: Midterm , Exam A,B
	thread_handles[0] = createThreadSimple(midtermGradeThread, grades_directory, &thread_ids[0]);
	if (thread_handles[0] == NULL)
	{
		printf("Failed to create thread.\n");
		CloseHandle(hw_mutex_handle);
		closeThreadHandles(thread_handles, thread_count);
		return TG_THREAD_CREATE_FAILED;
	}
	thread_count++;

	thread_handles[1] = createThreadSimple(getExamGradeThread, grades_directory, &thread_ids[1]);
	if (thread_handles[1] == NULL)
	{
		printf("Failed to create thread.\n");
		CloseHandle(hw_mutex_handle);
		closeThreadHandles(thread_handles, thread_count);
		return TG_THREAD_CREATE_FAILED;
	}
	thread_count++;

	// Homework grades threads
	for (i = 2; i < NUM_THREADS; i++)
	{
		// Create thread parameters
		thread_params[hw_id].grades_directory = grades_directory;
		thread_params[hw_id].hw_id = hw_id;
		thread_params[hw_id].hw_grades = hw_grades;
		thread_params[hw_id].hw_mutex_handle = hw_mutex_handle;
		
		thread_handles[i] = createThreadSimple(hwGradeThread, &(thread_params[hw_id]), &thread_ids[i]);
		hw_id++;
		if (thread_handles[i] == NULL)
		{
			printf("Failed to create thread.\n");
			CloseHandle(hw_mutex_handle);
			closeThreadHandles(thread_handles, thread_count);
			return TG_THREAD_CREATE_FAILED;
		}
		thread_count++;
	}

	// Wait for all threads to terminate
	wait_code = WaitForMultipleObjects(NUM_THREADS, thread_handles, true, INFINITE);
	if ((wait_code == WAIT_TIMEOUT) || (wait_code == WAIT_FAILED))
	{
		printf("Error waiting for threads to finish.\n");
		return TG_THREADS_WAIT_FAILED; // Should not return w/o cleanup
	}

	// Close mutex handle
	CloseHandle(hw_mutex_handle);

	// Close thread handles
	has_errors = closeThreadHandles(thread_handles, thread_count);
	if (has_errors == TRUE)
		return TG_THREAD_ERRORS_OCCURED;

	printf("hmidterm_grade %d\n", midterm_grade);
	printf("exam_grade %d\n", exam_grade);

	/*Calculate HW total grade*/
	hw_grade = calculateHomeworkGrade(hw_grades);
	printf("hw_grade %f\n", hw_grade);
	
	final_grade = calculateFinalGrade(hw_grade, midterm_grade, exam_grade);
	printf("final_grade %d\n", final_grade);

	// TODO: Print grade to file
	exit_code = writeFinalGradeToFile(grades_directory, final_grade);

	return exit_code;
}

EXIT_CODE writeFinalGradeToFile(const char *grades_directory, int final_grade)
{
	char *student_id;
	int id_start = '_';
	int filename_length = 0;
	char *final_grade_filename;
	EXIT_CODE exit_code;

	// Get the student's ID string
	student_id = strchr(grades_directory, id_start) + 1;

	filename_length = strlen(grades_directory) + 2 + FINAL_GRADE_FILENAME_LENGTH;
	final_grade_filename = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(final_grade_filename, filename_length, "%s//final_%s.txt", grades_directory, student_id);

	exit_code = writeToFile(final_grade_filename, final_grade);
	
	free(final_grade_filename);

	return exit_code;
}

EXIT_CODE createHWMutex(HANDLE mutex_handle)
{
	char hw_mutex_name[] = "hw_mutex";

	mutex_handle = createMutexSimple(hw_mutex_name);
	if (mutex_handle == NULL)
	{
		printf("Error creating mutex: %d\n", GetLastError());
		CloseHandle(mutex_handle);
		return ERROR_MUTEX_CREATE_FAILED;
	}

	return 0;
}

HANDLE createMutexSimple(LPCTSTR mutex_name)
{
	return CreateMutex(
		NULL,
		FALSE,
		mutex_name
	);
}

DWORD WINAPI hwGradeThread(LPVOID lpParam)
{
	hw_thread_params *thread_params;
	EXIT_CODE exit_code;

	Sleep(10);
	if (!lpParam)
		return TG_THREAD_PARAMS_NULL;

	thread_params = (hw_thread_params*)lpParam;

	exit_code = getHomeworkGrade(thread_params->grades_directory, thread_params->hw_id, thread_params->hw_grades, thread_params->hw_mutex_handle);
	return exit_code;
}

DWORD WINAPI midtermGradeThread(LPVOID lpParam)
{
	char* grades_directory;

	Sleep(10);
	grades_directory = (char*)lpParam;
	midterm_grade = getMidtermGrade(grades_directory);//GLOBAL
}

DWORD WINAPI getExamGradeThread(LPVOID lpParam)
{
	char* grades_directory;
	grades_directory = (char*)lpParam;
	exam_grade = getExamGrade(grades_directory);//GLOBAL
}

int getExamGrade(char* grades_directory)
{
	int moedA_grade;
	int moedB_grade;
	int exam_grade = 0;
	char* moedA_file_name = "moedA.txt";
	char* moedB_file_name = "moedB.txt";
	char* curr_file_path;
	int directory_path_length = strlen(grades_directory);
	int filename_length = directory_path_length + 1 + EXAM_FILENAME_LENGTH + 1;

	curr_file_path = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(curr_file_path, filename_length, "%s\\%s", grades_directory, moedA_file_name);
	moedA_grade = getGradeFromFile(curr_file_path);
	free(curr_file_path);

	/*code copied - insert to a function*/
	curr_file_path = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(curr_file_path, filename_length, "%s\\%s", grades_directory, moedB_file_name);
	moedB_grade = getGradeFromFile(curr_file_path);
	free(curr_file_path);
	exam_grade = moedB_grade;
	if (moedB_grade == 0)
		exam_grade = moedA_grade;
	if (exam_grade < FAIL_THRESHOLD)
		exam_grade = 0;
	return (exam_grade);
}

int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade)
{
	float final_grade = 0;
	final_grade = 0.2*hw_grade + 0.2*midterm_grade + 0.6*exam_grade;
	return ceil(final_grade);
}

BOOL closeThreadHandles(HANDLE thread_handles[], int thread_count)
{
	BOOL has_errors = FALSE;
	int i = 0;
	int thread_exit_code = 0;

	for (i = 0; i < thread_count; i++)
	{
		GetExitCodeThread(thread_handles[i], &thread_exit_code);
		printf("Thread %d ended with exit code 0x%x\n", i, thread_exit_code);
		
		if (thread_exit_code != TG_SUCCESS)
			has_errors = TRUE;

		CloseHandle(thread_handles[i]);
	}

	return has_errors;
}