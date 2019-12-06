#define MIDTERM_FILENAME_LENGTH 11

int getMidtermGrade(char* grades_directory)
{
	int midterm_grade;
	char* midterm_file_name = "midterm.txt";
	char* curr_file_path;
	int directory_path_length = strlen(grades_directory);
	int filename_length = directory_path_length + 1 + MIDTERM_FILENAME_LENGTH + 1;

	curr_file_path = (char*)malloc(sizeof(char)*filename_length);
	sprintf_s(curr_file_path, filename_length, "%s\\%s", grades_directory, midterm_file_name);
	midterm_grade = getGradeFromFile(curr_file_path);
	if (midterm_grade < FAIL_THRESHOLD)
		midterm_grade = 0;
	free(curr_file_path);
	return midterm_grade;
}