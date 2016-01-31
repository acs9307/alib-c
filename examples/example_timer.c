#include <stdio.h>
#include <unistd.h>

#include "Timer.h"
#include "TimerLight.h"

/* Runs the both timer types for 1 second each and prints the number
 * of times it must check the timer before it rings.  Always should
 * print 0. */

void Timer_test()
{
	size_t i;
	Timer* t = newTimer(MICROS_PER_SECOND, MICROS);
	if(!t)
	{
		printf("Error occurred allocating memory for the timer.\n");
		return;
	}

	Timer_begin(t);
	usleep(Timer_get_run_time(t));
	for(i = 0; !Timer_check(t); ++i);
	printf("Timer check %lu times.\n", i);

	Timer_set_magnitude(t, MILLIS);
	Timer_begin(t);
	usleep(MILLIS_TO_MICROS(Timer_get_run_time(t)));
	for(i = 0; !Timer_check(t); ++i);
	printf("Timer check %lu times.\n", i);

	Timer_set_magnitude(t, SECONDS);
	Timer_begin(t);
	usleep(SECONDS_TO_MICROS(Timer_get_run_time(t)));
	for(i = 0; !Timer_check(t); ++i);
	printf("Timer check %lu times.\n", i);


	delTimer(&t);
	/* Could also do. */
//	delTimerBase((TimerBase**)&t);
}
void TimerLight_test()
{
	size_t i;
	TimerLight* t = newTimerLight(MICROS_PER_SECOND, MICROS);
	if(!t)
	{
		printf("Error occurred allocating memory for the timer.\n");
		return;
	}

	TimerLight_begin(t);
	usleep(TimerLight_get_run_time(t));
	for(i = 0; !TimerLight_check(t); ++i);
	printf("TimerLight check %lu times.\n", i);

	TimerLight_set_magnitude(t, MILLIS);
	TimerLight_begin(t);
	usleep(MILLIS_TO_MICROS(TimerLight_get_run_time(t)));
	for(i = 0; !TimerLight_check(t); ++i);
	printf("TimerLight check %lu times.\n", i);

	TimerLight_set_magnitude(t, SECONDS);
	TimerLight_begin(t);
	usleep(SECONDS_TO_MICROS(TimerLight_get_run_time(t)));
	for(i = 0; !TimerLight_check(t); ++i);
	printf("TimerLight check %lu times.\n", i);


	delTimerLight(&t);
	/* Could also do. */
//	delTimerBase((TimerBase**)&t);
}

int main()
{
	Timer_test();
	printf("\n");
	TimerLight_test();

	printf("Application finished!\n");
	return(0);
}
