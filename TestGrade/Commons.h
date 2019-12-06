#ifndef COMMONS_H
#define COMMONS_H

typedef int EXIT_CODE;

#define FAIL_THRESHOLD 60
#define NUM_OF_HW 10

#define ERROR_WAITING_FOR_THREADS ((int)(-1))
#define ERROR_CLOSING_THREAD ((int)(-2))
#define ERROR_OPEN_FILE ((int)(-3))
#define ERROR_LPPARAM_NULL ((int)(-4))
#define ERROR_CREATING_THREAD ((int)(-5))
#define ERROR_MUTEX_CREATE_FAILED -6
#define ERROR_MUTEX_ABANDONED -7
#define ERROR_MUTEX_WAIT_FAILED -8
#define ERROR_MUTEX_RELEASE_FAILED -9;

#endif