#ifndef THREAD_HANDLE_H
#define THREAD_HANDLE_H

#include <windows.h>

HANDLE createThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPVOID p_thread_parameters,
	LPDWORD p_thread_id);

#endif
