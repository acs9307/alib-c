#include <stdio.h>
#include <unistd.h>

#include "String.h"
#include "Timer.h"
#include "TimerEvent.h"

/* Runs a TimerEventHandler for 10 seconds. */

void timer_rang(TimerEvent* event)
{
	static int i = 0;
	printf("Timer rang %d times!\n", ++i);
}

int main()
{
	TimerEvent* event = newTimerEvent(1, 0, timer_rang, "hello world", NULL);
	int i = 0;
	char rang;
	struct timespec time, rTime;

	TimerEvent_begin(event);
	for(i = 0; i < 10;)
	{
		time = TimerEvent_check(event, &rang);
		if(!rang)
		{
			Timer* t = TimerEvent_get_timer(event);
			Timer_get_time_status(t, &rTime, NULL);
			printf("\tRemaining time: %ld.%.9ld seconds\n", rTime.tv_sec, rTime.tv_nsec);
			printf("\tEnd time: %ld.%.9ld seconds system time\n\n", time.tv_sec, time.tv_nsec);
		}
		else
			++i;

		usleep(100 * 1000);
	}

	delTimerEvent(&event);
	printf("Application finished!\n");
	return(0);
}
