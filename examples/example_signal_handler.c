#include "StringObject.h"
#include "signal_handler.h"

#include <unistd.h>

char sigint_called = 0;
char sigabort_called = 0;
void sig_handler(int signum, void* user_data)
{
	StringObject* str = (StringObject*)user_data;

	printf("%d: %s\n", signum, str->str);

	++sigint_called;
//	if(sigint_called > 1)
//		abort();
}
void sig_abort(int signum, void* user_data)
{
	StringObject* str = (StringObject*)user_data;

	printf("%d: %s\n", signum, str->str);
	sigabort_called++;
}

int main()
{
	StringObject* str1 = newStringObject();
	StringObject* str2 = newStringObject();
	StringObject* str3 = newStringObject();

	str1->append(str1, "Hello world!");
	str2->append(str2, "Hey there!");
	str3->append(str3, "Abort called!");

	signal_handler_register(SIGINT, sig_handler, str1);
	signal_handler_register(SIGINT, sig_handler, str2);
//	signal_handler_register(SIGABRT, sig_abort, str3);
	signal_handler_register(SIGINT, sig_handler, str3);

	while(!sigint_called)
		sleep(1);

	signal_handler_deregister(SIGINT, sig_handler, str2);

	sigabort_called = 0;
	sigint_called = 0;
	while(!sigint_called)
		usleep(1);
	sleep(1);

	return(0);
}
