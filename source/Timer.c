#include "Timer_private.h"

/*******Public Functions*******/
void Timer_begin(Timer* t)
{
	if(!t)return;

	struct timespec time;
	
	clock_gettime(CLOCK_MONOTONIC, &time);
	
	t->start_time = time.tv_sec * 1000;
	t->start_time += time.tv_nsec / (1000 * 1000);

	t->rang = 0;
}

char Timer_check(Timer* t)
{
	if(!t)return(ALIB_BAD_ARG);
	if(t->rang)return(t->rang);
	
	struct timespec time;
	uint64_t millis, ellapsed;
	clock_gettime(CLOCK_MONOTONIC, &time);
	millis = time.tv_sec * 1000;
	millis += time.tv_nsec / (1000 * 1000);
	
	if(millis < t->start_time)
	{
		ellapsed = UINT64_MAX - t->start_time;
		ellapsed += millis;
	}
	else
		ellapsed = millis - t->start_time;
	
	if(ellapsed >= t->run_time)
		t->rang = 1;
	
	return(t->rang);
}

	/* Setters */
void Timer_set_run_time(Timer* t, uint64_t run_time){t->run_time = run_time;}
	/***********/
/******************************/

/*******Constructors*******/
Timer* newTimer(uint64_t run_time)
{
	Timer* t = malloc(sizeof(Timer));
	if(!t)return(NULL);
	
	t->start_time = 0;
	t->run_time = run_time;
	t->rang = 1;
	
	return(t);
}
void delTimer(Timer** t)
{
	if(!t || !*t)return;
	
	free(*t);
	*t = NULL;
}
/**************************/