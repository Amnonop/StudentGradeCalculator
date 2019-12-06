#include <stdlib.h>
#include <limits.h>
#include <stdio.h> 
#include <string.h> 
#include <errno.h>
#include <math.h>
#include <windows.h>
#include "TestGrade.h"
#include "Commons.h"
#include "FileHandle.h"
#include "ThreadHandle.h"

#define HW_FILENAME_LENGTH 9
#define NUM_OF_CALC_HW 8
#define EXAM_FILENAME_LENGTH 9
#define NUM_THREADS 13
#define BRUTAL_TERMINATION_CODE 0x55

/*global variables*/
static int midterm_grade = 0;
static int exam_grade = 0;
static int hw_grades[NUM_OF_HW] = { 0 };
//static int hw_grades[NUM_OF_HW] = { 0 };
HANDLE p_thread_handles[NUM_THREADS];
DWORD p_thread_ids[NUM_THREADS];
DWORD wait_code;
BOOL ret_val;
size_t i;
char* hw_file_names[NUM_OF_HW] =
{ "ex01.txt","ex02.txt","ex03.txt",
"ex04.txt", "ex05.txt", "ex06.txt",
"ex07.txt", "ex08.txt", "ex09.txt", "ex10.txt" };

/*declerations*/
float getHomeWorkGrade(char* grades_directory);
void sortArray(int* hw_grades[NUM_OF_HW]);
int getMidtermGrade(char* grades_directory);
int getExamGrade(char* grades_directory);
int calculateFinalGrade(float hw_grade, int midterm_grade, int exam_grade);
DWORD WINAPI midtermGradeThread(LPVOID lpParam);
DWORD WINAPI getExamGradeThread(LPVOID lpParam);
DWORD WINAPI hwGradeThread(LPVOID lpParam);

int calculateGrade(char* grades_directory)
{
	/*10 HW*/
	/*MIDTERM*/
	/*FINAL A*/
	/*FINAL B*/
	// Create two threads: Midterm , Exam A,B
	p_thread_handles[0] = createThreadSimple(midtermGradeThread, grades_directory, &p_thread_ids[0]);
	p_thread_handles[1] = createThreadSimple(getExamGradeThread, grades_directory, &p_thread_ids[1]);
	//for (size_t i = 2; i < NUM_THREADS; i++)
	//{
	//	p_thread_handles[i] = createThreadSimple(midtermGradeThread, grades_directory, &p_thread_ids[i]);
	//}
	/*Calculate HW total grade*/
	float hw_grade = getHomeWorkGrade(grades_directory);
	printf("hw_grade %f", hw_grade);
	//int midterm_grade = getMidtermGrade(grades_directory);
	printf("hmidterm_grade %d", midterm_grade);
	//int exam_grade = getExamGrade(grades_directory);
	printf("exam_grade %d", exam_grade);
	int final_grade = calculateFinalGrade(hw_grade, midterm_grade, exam_grade);
	printf("final_grade %d", final_grade);
}

float getHomeWorkGrade(char* grades_directory)
{
	int i = 0;
	char* curr_file_path;
	int directory_path_length = strlen(grades_directory);
	int filename_length = directory_path_length + 1 + HW_FILENAME_LENGTH + 1;
	for (int i = 0; i < NUM_OF_HW; i++)
	{
		curr_file_path = (char*)malloc(sizeof(char)*filename_length);
		sprintf_s(curr_file_path, filename_length, "%s\\%s", grades_directory, hw_file_names[i]);
		p_thread_handles[i + 2] = (int)createThreadSimple(getGradeFromFile, grades_directory, &p_thread_ids[i + 2]);
		//hw_grades[i] = getGradeFromFile(curr_file_path);
		free(curr_file_path);
	}

	sortArray(hw_grades);
	float hw_grade = 0;

	for (int i = 0; i < NUM_OF_CALC_HW; i++)
	{
		if (hw_grades[i] >= FAIL_THRESHOLD)
			hw_grade += hw_grades[i];
	}
	return (hw_grade / NUM_OF_CALC_HW);
}

DWORD WINAPI hwGradeThread(LPVOID lpParam)
{
	char* grades_directory;
	int curr_hw_grade = 0;
	grades_directory = (char*)lpParam;
	curr_hw_grade = getGradeFromFile(grades_directory);//GLOBAL
	return (curr_hw_grade);
}

DWORD WINAPI midtermGradeThread(LPVOID lpParam)
{
	char* grades_directory;
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
/*void appendToFile(char* file_path, char* sub_solution)
{
	FILE *fp;
	fopen_s(&fp, file_path, "a");

	if (!fp)
		return;

	char* new_line;
	int new_line_length;
	new_line_length = strlen(sub_solution) + 2;
	new_line = (char*)malloc(sizeof(char) * new_line_length);
	strcpy_s(new_line, new_line_length, sub_solution);
	strcat_s(new_line, new_line_length, "\n");

	fprintf(fp, "%s", new_line);
	fclose(fp);

	free(new_line);
}*/

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