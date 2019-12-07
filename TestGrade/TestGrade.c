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
#define FINAL_GRADE_FILENAME_LENGTH 19

/*declerations*/
int getExamGrade(char* grades_directory);
int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade);
DWORD WINAPI midtermGradeThread(LPVOID lpParam);
DWORD WINAPI getExamGradeThread(LPVOID lpParam);

DWORD WINAPI hwGradeThread(LPVOID lpParam);

EXIT_CODE createHWMutex(HANDLE mutex_handle);
HANDLE createMutexSimple(LPCTSTR mutex_name);

EXIT_CODE writeFinalGradeToFile(const char *grades_directory, int final_grade);

typedef struct _hw_thread_params
{
	char *grades_directory;
	int hw_id;
	int *hw_grades;
	HANDLE hw_mutex_handle;
} hw_thread_params;

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
int calculateGrade(char* grades_directory)
{
	char hw_mutex_name[] = "hw_mutex";
	HANDLE hw_mutex_handle;
	HANDLE p_thread_handles[NUM_THREADS];
	DWORD p_thread_ids[NUM_THREADS];
	int hw_grades[NUM_OF_HW] = { 0 };
	hw_thread_params thread_params[NUM_OF_HW];
	DWORD wait_code;
	BOOL ret_val;
	EXIT_CODE exit_code;
	int i = 0;
	int hw_id = 0;
	float hw_grade = 0.0;
	int final_grade = 0;

	// Create mutex
	hw_mutex_handle = createMutexSimple(hw_mutex_name);
	if (hw_mutex_handle == NULL)
	{
		printf("Error creating HW mutex: %d\n", GetLastError());
		CloseHandle(hw_mutex_handle);
		return TG_MUTEX_CREATE_FAILED;
	}

	// Create two threads: Midterm , Exam A,B
	p_thread_handles[0] = createThreadSimple(midtermGradeThread, grades_directory, &p_thread_ids[0]);
	p_thread_handles[1] = createThreadSimple(getExamGradeThread, grades_directory, &p_thread_ids[1]);

	// Homework grades threads
	for (i = 2; i < NUM_THREADS; i++)
	{
		// Create thread parameters
		thread_params[hw_id].grades_directory = grades_directory;
		thread_params[hw_id].hw_id = hw_id;
		thread_params[hw_id].hw_grades = hw_grades;
		thread_params[hw_id].hw_mutex_handle = hw_mutex_handle;
		
		p_thread_handles[i] = createThreadSimple(hwGradeThread, &(thread_params[hw_id]), &p_thread_ids[i]);
		hw_id++;
		if (p_thread_handles[i] == NULL)
		{
			// TODO: Before returning should close all open handles!!!
			return TG_THREAD_CREATE_FAILED;
		}
	}

	// Wait for all threads to terminate
	wait_code = WaitForMultipleObjects(NUM_THREADS, p_thread_handles, true, INFINITE);
	if ((wait_code == WAIT_TIMEOUT) || (wait_code == WAIT_FAILED))
	{
		printf("Error waiting for threads to finish.\n");
		return TG_THREADS_WAIT_FAILED; // Should not return w/o cleanup
	}

	/*Calculate HW total grade*/
	hw_grade = calculateHomeworkGrade(hw_grades);
	
	final_grade = calculateFinalGrade(hw_grade, midterm_grade, exam_grade);

	// Close thread handles
	for (i = 0; i < NUM_THREADS; i++)
	{
		ret_val = CloseHandle(p_thread_handles[i]);
		if (!ret_val)
		{
			printf("Error when closing thread.\n");
			return ERROR_CLOSING_THREAD;
		}
	}

	// Close mutex handle
	CloseHandle(hw_mutex_handle);

	// TODO: Print grade to file
	exit_code = writeFinalGradeToFile(grades_directory, final_grade);

	return exit_code;
}

/**
*	Writes the final grade of the student to a file.
*
*	Accepts
*	-------
*	grades_directory - a string representing the name of the directory containing the file.
*   final_grade - an integer containing the final grade of the student.
*
*	Returns
*	-------
*	An EXIT_CODE inidcating wether the operation was succefull.
**/
EXIT_CODE writeFinalGradeToFile(const char *grades_directory, int final_grade)
{
	char *student_id;
	int id_start = '_';
	int filename_length = 0;
	char *final_grade_filename;
	EXIT_CODE exit_code;

	// Get the student's ID string
	student_id = grades_directory + (strlen(grades_directory) - ID_LENGTH); //strchr(grades_directory, id_start) + 1;

	filename_length = strlen(grades_directory) + 2 + FINAL_GRADE_FILENAME_LENGTH;
	final_grade_filename = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(final_grade_filename, filename_length, "%s\\final_%s.txt", grades_directory, student_id);

	exit_code = writeToFile(final_grade_filename, final_grade);
	
	free(final_grade_filename);

	return exit_code;
}

/**
*	Created a mutex to the homework array.
*
*	Accepts
*	-------
*	mutex_handle - a pointer to be assigned with the created handle.
*
*	Returns
*	-------
*	An exit code indicating if the action was successfull.
**/
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

/**
*	Returns a handle to a mutex.
*
*	Accepts
*	-------
*	mutex_name - a string containing a unique identifier to the mutex.
*
*	Returns
*	-------
*	A handle to a mutex if created successfully, otherwise returns NULL.
**/
HANDLE createMutexSimple(LPCTSTR mutex_name)
{
	return CreateMutex(
		NULL,
		FALSE,
		mutex_name
	);
}

/**
*	Runs the reading of a homework grade in a thread.
*
*	Accepts
*	-------
*	lpParam - the parameters to the thread.
*
*	Returns
*	-------
*	An exit code.
**/
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

/**
*	Runs the reading of the midterm grade in a thread.
*
*	Accepts
*	-------
*	lpParam - the parameters to the thread.
*
*	Returns
*	-------
*	An exit code.
**/
DWORD WINAPI midtermGradeThread(LPVOID lpParam)
{
	char* grades_directory;

	Sleep(10);
	grades_directory = (char*)lpParam;
	midterm_grade = getMidtermGrade(grades_directory);//GLOBAL
}

/**
*	Runs the reading of the exam grade in a thread.
*
*	Accepts
*	-------
*	lpParam - the parameters to the thread.
*
*	Returns
*	-------
*	An exit code.
**/
DWORD WINAPI getExamGradeThread(LPVOID lpParam)
{
	char* grades_directory;
	Sleep(10);
	grades_directory = (char*)lpParam;
	exam_grade = getExamGrade(grades_directory);//GLOBAL
}

/**
*	Reads the exam grade from the file. The last grade is the one to be returned.
*
*	Accepts
*	-------
*	grades_directory - a string representing the name of the directory containing the file.
*
*	Returns
*	-------
*	An integer containing the grade of the final exam.
**/
int getExamGrade(char* grades_directory)
{
	int moedA_grade;
	int moedB_grade;
	int exam_grade = 0;
	char* moedA_file_name = "moedA.txt";
	char* moedB_file_name = "moedB.txt";
	char* curr_file_path;
	int directory_path_length = strlen(grades_directory);
	int filename_length = directory_path_length + 2 + EXAM_FILENAME_LENGTH + 1;

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

/**
*	Calculates the final grade. The final grade is rounded up.
*
*	Accepts
*	-------
*	hw_grade - a float containing the final hw grade.
*	midterm_grade - an integer containing the midterm grade.
*	exam_grade - an integer containing the exam grade.
*
*	Returns
*	-------
*	An integer containing the final grade of the student.
**/
int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade)
{
	float final_grade = 0;
	final_grade = 0.2*hw_grade + 0.2*midterm_grade + 0.6*exam_grade;
	return ceil(final_grade);
}