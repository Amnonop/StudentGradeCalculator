#ifndef HOMEWORK_GRADE_H
#define HOMEWORK_GRADE_H

#include <windows.h>
#include "Commons.h"

EXIT_CODE getHomeworkGrade(char* grades_directory, int hw_id, int hw_grades[], HANDLE hw_mutex_handle);

float calculateHomeworkGrade(int grades[]);

#endif
