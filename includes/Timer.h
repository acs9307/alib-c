#ifndef TIMER_IS_DEFINED
#define TIMER_IS_DEFINED

#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

typedef struct Timer Timer;

/*******Public Functions*******/
void Timer_begin(Timer* t);

char Timer_check(Timer* t);

	/* Setters */
void Timer_set_run_time(Timer* t, uint64_t run_time);
	/***********/
/******************************/

/*******Constructors*******/
Timer* newTimer(uint64_t run_time);
void delTimer(Timer** t);
/**************************/

#endif