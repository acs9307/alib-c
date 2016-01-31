#include "TimerBase_private.h"

/*******Destructors*******/
/* Frees a timer object. */
void freeTimerBase(TimerBase* t)
{
	if(t)
		free(t);
}
/* Deletes a timer object. */
void delTimerBase(TimerBase** t)
{
	if(!t || !*t)return;

	freeTimerBase(*t);
	*t = NULL;
}
/*************************/
