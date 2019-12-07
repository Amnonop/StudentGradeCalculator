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

#define HW_FILENAME_LENGTH 9
#define NUM_OF_CALC_HW 8
#define EXAM_FILENAME_LENGTH 9
#define NUM_THREADS 12
#define BRUTAL_TERMINATION_CODE 0x55
#define ID_LENGTH 9
#define FINAL_GRADE_FILENAME_LENGTH 20

/*global variables*/
static int midterm_grade = 0;
static int exam_grade = 0;
static int hw_grades[NUM_OF_HW] = { 0 };

char* hw_file_names[NUM_OF_HW] =
{ "ex01.txt","ex02.txt","ex03.txt",
"ex04.txt", "ex05.txt", "ex06.txt",
"ex07.txt", "ex08.txt", "ex09.txt", "ex10.txt" };

/*declerations*/
float calculateHomeworkGrade(int homework_grades[]);
void sortArray(int* hw_grades[NUM_OF_HW]);
int getExamGrade(char* grades_directory);
int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade);
DWORD WINAPI midtermGradeThread(LPVOID lpParam);
DWORD WINAPI getExamGradeThread(LPVOID lpParam);

DWORD WINAPI hwGradeThread(LPVOID lpParam);
EXIT_CODE getHomeworkGrade(char* grades_directory, int hw_id, HANDLE hw_mutex_handle);
EXIT_CODE updateHWGrade(int hw_id, int hw_grade, HANDLE hw_mutex_handle);

EXIT_CODE readGradeFromFile(const char *grades_directory, const char *grade_filename, int *grade);

EXIT_CODE createHWMutex(HANDLE mutex_handle);
HANDLE createMutexSimple(LPCTSTR mutex_name);

EXIT_CODE writeFinalGradeToFile(const char *grades_directory, int final_grade);

typedef struct _hw_thread_params
{
	char *grades_directory;
	int hw_id;
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
	HANDLE p_thread_handles[NUM_THREADS];
	DWORD p_thread_ids[NUM_THREADS];
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

	printf("hmidterm_grade %d\n", midterm_grade);
	printf("exam_grade %d\n", exam_grade);

	/*Calculate HW total grade*/
	hw_grade = calculateHomeworkGrade(hw_grades);
	printf("hw_grade %f\n", hw_grade);
	
	final_grade = calculateFinalGrade(hw_grade, midterm_grade, exam_grade);
	printf("final_grade %d\n", final_grade);

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

float calculateHomeworkGrade(int homework_grades[])
{
	int i = 0;
	float hw_grade = 0;

	sortArray(homework_grades);

	for (i = 0; i < NUM_OF_CALC_HW; i++)
	{
		hw_grade += homework_grades[i];
	}

	return (hw_grade / NUM_OF_CALC_HW);
}

DWORD WINAPI hwGradeThread(LPVOID lpParam)
{
	hw_thread_params *thread_params;
	EXIT_CODE exit_code;

	Sleep(10);
	if (!lpParam)
		return TG_THREAD_PARAMS_NULL;

	thread_params = (hw_thread_params*)lpParam;

	exit_code = getHomeworkGrade(thread_params->grades_directory, thread_params->hw_id, thread_params->hw_mutex_handle);
	return exit_code;
}

EXIT_CODE getHomeworkGrade(char* grades_directory, int hw_id, HANDLE hw_mutex_handle)
{
	int hw_grade = 0;
	EXIT_CODE exit_code;

	exit_code = readGradeFromFile(grades_directory, hw_file_names[hw_id], &hw_grade);

	if (exit_code != 0)
		return exit_code;

	if (hw_grade < FAIL_THRESHOLD)
		hw_grade = 0;

	// This section should be locked
	exit_code = updateHWGrade(hw_id, hw_grade, hw_mutex_handle);
	if (exit_code != 0)
		return exit_code;

	return TG_SUCCESS;
}

EXIT_CODE updateHWGrade(int hw_id, int hw_grade, HANDLE hw_mutex_handle)
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

EXIT_CODE readGradeFromFile(const char *grades_directory, const char *grade_filename, int *grade)
{
	char *grade_file_path;
	int filename_length;
	EXIT_CODE EXIT_CODE;

	filename_length = strlen(grades_directory) + 2 + strlen(grade_filename) + 1;
	grade_file_path = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(grade_file_path, filename_length, "%s//%s", grades_directory, grade_filename);

	EXIT_CODE = readFromFile(grade_file_path, grade);

	free(grade_file_path);
	
	return EXIT_CODE;
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

int getGradeFromFile2(char* filename)
{
	/*Ecactly the same as above but for the HWThread*/
	int sub_grade = 0;
	FILE *fp;
	errno_t error;
	error = fopen_s(&fp, filename, "r");

	if (error != 0)
		printf("An error occured while openning file %s for writing.", filename);

	else if (fp)
	{
		fscanf_s(fp, "%d", &sub_grade);
		fclose(fp);
	}

	return sub_grade;
}

int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade)
{
	float final_grade = 0;
	final_grade = 0.2*hw_grade + 0.2*midterm_grade + 0.6*exam_grade;
	return ceil(final_grade);
}

void sortArray(int* hw_grades[NUM_OF_HW])
{
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
}