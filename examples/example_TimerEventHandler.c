#include <stdio.h>
#include <unistd.h>

#include "TimerEventHandler_private.h"

struct event_obj
{
	char* str;
	int times_left;
};

void timer_event(TimerEvent* event)
{
	struct event_obj* obj = (struct event_obj*)TimerEvent_get_ex_data(event);
	printf("%s\n", obj->str);

	--obj->times_left;

	if(!obj->times_left)
		delTimerEvent(&event);
}

int main()
{
	TimerEventHandler* handler = newTimerEventHandler(0);
	struct event_obj array[] = {{"10 second", 1}, {"1 second", 10}, {"100 millis", 10 * 10}};
	struct event_obj* one_millis = (struct event_obj*)malloc(sizeof(struct event_obj));
	*one_millis = (struct event_obj){"1 millis", 1000 * 10};

	printf("add_tsafe: %d\n", TimerEventHandler_add_tsafe(handler, newTimerEvent(10, 0, timer_event, array, NULL)));
	printf("add_tsafe: %d\n", TimerEventHandler_add_tsafe(handler, newTimerEvent(1, 0, timer_event, array + 1, NULL)));
	//printf("add_tsafe: %d\n", TimerEventHandler_add_tsafe(handler, newTimerEvent(0, MILLIS_TO_NANOS(1), timer_event, one_millis, free)));
	printf("add_tsafe: %d\n", TimerEventHandler_add_tsafe(handler, newTimerEvent(0, MILLIS_TO_NANOS(100), timer_event, array + 2, NULL)));

	{
		DListItem* it;
		printf("Handler's event list:\n");
		for(it = DList_get(handler->list, 0); it; it = DListItem_get_next_item(it))
		{
			struct event_obj* it_val = (struct event_obj*)TimerEvent_get_ex_data((TimerEvent*)DListItem_get_value(it));
			printf("\t%s\n", it_val->str);
		}
		printf("\n\n");
	}

	printf("Starting TimerEventHandler.\n");
	TimerEventHandler_start(handler);

	while(TimerEventHandler_get_event_count(handler))
		sleep(1);


	printf("Application complete!\n");
	return(0);
}
