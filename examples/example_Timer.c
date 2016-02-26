#include <stdio.h>
#include <unistd.h>

#include "Timer.h"

/* Runs the timer for 1 second and prints the number
 * of times it must check the timer before it rings.  Always should
 * print 0. */

void Timer_test()
{
	size_t i;
	const struct timespec* rTime;
	Timer* t = newTimer(1, 0);
	if(!t)
	{
		printf("Error occurred allocating memory for the timer.\n");
		return;
	}

	Timer_begin(t);
	rTime = Timer_get_run_time(t);
	usleep(NANOS_TO_MICROS(rTime->tv_nsec) + SECONDS_TO_MICROS(rTime->tv_sec));
	for(i = 0; !Timer_check(t); ++i);
	printf("Timer check %lu times.\n", i);

	Timer_begin(t);
	for(i = 0; !Timer_check(t); ++i)
		usleep((NANOS_TO_MICROS(rTime->tv_nsec) + SECONDS_TO_MICROS(rTime->tv_sec)) / 10);
	printf("Timer check %lu times.\n", i);

	delTimer(&t);
}

int main()
{
	Timer_test();
	printf("\n");

	printf("Application finished!\n");
	return(0);
}
