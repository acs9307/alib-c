#include <stdio.h>
#include <unistd.h>

#include "ThreadedTimerEvent.h"

/* Example that shows how to use a ThreadedTimerEvent. */

void timer_event(ThreadedTimerEvent* event)
{
	static int timesLeft = 10;

	printf("%s\n", (char*)TimerEvent_get_ex_data((TimerEvent*)event));

	--timesLeft;

	if(!timesLeft)
		ThreadedTimerEvent_stop_async(event);
}

int main()
{
	ThreadedTimerEvent* event = newThreadedTimerEvent(1, 0, (TimerEvent_rang_cb)timer_event, "1 second", NULL);
	printf("ThreadedTimerEvent_start(): %d\n", ThreadedTimerEvent_start(event));

	while(ThreadedTimerEvent_is_running(event))
		ThreadedTimerEvent_wait(event);


	printf("Application complete!\n");
	return(0);
}
