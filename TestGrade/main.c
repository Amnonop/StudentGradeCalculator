#include <stdlib.h>
#include <limits.h>
# include <stdio.h> 
# include <string.h> 
# include <errno.h>
#define FAIL_THRESHOLD 60
# define NUM_OF_HW 10
# define HW_FILENAME_LENGTH 9
#define NUM_OF_CALC_HW 8

int getGradeFromFile(char* filename);
int getHomeWorkGrade(char* grades_directory);
void sortArray(int* hw_grades[NUM_OF_HW]);

int main()
{
	char* grades_directory = "C:\\Users\\ophir\\source\\repos\\StudentGradeCalculator\\Debug\\grades_123456789";
	/*10 HW*/
	/*MIDTERM*/
	/*FINAL A*/
	/*FINAL B*/

	/*Calculate HW total grade*/
	int hw_grade = getHomeWorkGrade(grades_directory);
	printf("hw_grade %d", &hw_grade);
}

int getHomeWorkGrade(char* grades_directory)
{
	int hw_grades[NUM_OF_HW] = { 0 };
	int i = 0;
	char* hw_file_names[NUM_OF_HW] = 
		{"ex01.txt","ex02.txt","ex03.txt", 
		"ex04.txt", "ex05.txt", "ex06.txt",
		"ex07.txt", "ex08.txt", "ex09.txt", "ex10.txt" };
	char* curr_file_path;

	int directory_path_length = strlen(grades_directory);
	int filename_length = directory_path_length + 1 + HW_FILENAME_LENGTH + 1;

	for (int i = 0; i < NUM_OF_HW; i++)
	{
		curr_file_path = (char*)malloc(sizeof(char)*filename_length);
		sprintf_s(curr_file_path, filename_length, "%s\\%s", grades_directory, hw_file_names[i]);
		
		hw_grades[i] = getGradeFromFile(curr_file_path);
		free(curr_file_path);
	}

	sortArray(hw_grades);
	int hw_grade = 0;

	for (int i = 0; i < NUM_OF_CALC_HW; i++)
	{
		if (hw_grades[i] >= FAIL_THRESHOLD) 
			hw_grade += hw_grades[i];
	}
	return hw_grade / NUM_OF_CALC_HW;
}

int getGradeFromFile(char* filename)
{
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


void appendToFile(char* file_path, char* sub_solution)
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