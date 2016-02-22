#include "proc_waiter.h"
#include "StringObject.h"

void proc_wait_cb(int pid, int status, void* user_data)
{
	StringObject* str = (StringObject*)user_data;
	printf("%s\n", str->str);
}

void test1()
{
	printf("Test 1 started!\n");

	StringObject* all = newStringObject();
	StringObject* first = newStringObject();
	StringObject* second = newStringObject();
	int pid1, pid2;

	all->append(all, "A child processed died.");
	first->append(first, "The first child process died.");
	second->append(second, "The second child process died.");

	proc_waiter_register(-1, proc_wait_cb, all);

	pid1 = fork();
	if(!pid1)
	{
		usleep(1000);
		exit(0);
	}
	proc_waiter_register(pid1, proc_wait_cb, first);
	usleep(1000);

	pid2 = fork();
	if(!pid2)
	{
		usleep(1000);
		exit(0);
	}
	proc_waiter_register(pid2, proc_wait_cb, second);
	sleep(1);

	proc_waiter_deregister_all();
	proc_waiter_stop();
	printf("Test 1 complete!\n\n\n");
}

void test2()
{
	printf("Test 2 started!\n");
	int i = 0;

	proc_waiter_start();
	for(; i < 1000; ++i)
	{
		if(!fork())
			exit(0);
		else if(!(i % 100))
			printf("\tTest 2: %d forks complete!\n", i);
	}

	sleep(1);
	proc_waiter_stop();
	printf("Test 2 complete!\n\n\n");
}

void test3()
{
	printf("Test 3 started!\n");

	StringObject* all = newStringObject();
	StringObject* first = newStringObject();
	StringObject* second = newStringObject();
	int pid1, pid2;

	all->append(all, "A child processed died.");
	first->append(first, "A child processed died.2");
	second->append(second, "A child processed died.3");

	proc_waiter_register(-1, proc_wait_cb, all);
	proc_waiter_register(-1, proc_wait_cb, first);
	proc_waiter_register(-1, proc_wait_cb, second);

	pid1 = fork();
	if(!pid1)
	{
		usleep(1000);
		exit(0);
	}
	usleep(10000);

	proc_waiter_deregister(-1, NULL, first);

	pid2 = fork();
	if(!pid2)
	{
		usleep(1000);
		exit(0);
	}
	sleep(1);

	proc_waiter_deregister_all();
	proc_waiter_stop();
	printf("Test 3 complete!\n\n\n");
}

int main()
{
	test1();
	test2();
	test3();

	return(0);
}
