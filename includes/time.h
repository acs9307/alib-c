#ifndef ALIB_TIME_IS_DEFINED
#define ALIB_TIME_IS_DEFINED

/*******Conversion Defines*******/
	/* To Seconds */
#ifndef MILLIS_PER_SECOND
#define MILLIS_PER_SECOND (1000)
#endif
#ifndef MICROS_PER_SECOND
#define MICROS_PER_SECOND (1000 * MILLIS_PER_SECOND)
#endif
#ifndef NANOS_PER_SECOND
#define NANOS_PER_SECOND (1000 * MICROS_PER_SECOND)
#endif
#ifndef PICOS_PER_SECOND
#define PICOS_PER_SECOND \
	(uint64_t)((uint64_t)1000 * (uint64_t)NANOS_PER_SECOND)
#endif
	/**************/

	/* To Millis */
#ifndef MICROS_PER_MILLIS
#define MICROS_PER_MILLIS (1000)
#endif
#ifndef NANOS_PER_MILLIS
#define NANOS_PER_MILLIS (1000 * MICROS_PER_MILLIS)
#endif
#ifndef PICOS_PER_MILLIS
#define PICOS_PER_MILLIS (1000 * NANOS_PER_MILLIS)
#endif
	/*************/

	/* To Micros */
#ifndef NANOS_PER_MICROS
#define NANOS_PER_MICROS (1000)
#endif
#ifndef PICOS_PER_MICROS
#define PICOS_PER_MICROS (1000 * NANOS_PER_MICROS)
#endif
	/*************/

	/* To Nanos */
#ifndef PICOS_PER_NANOS
#define PICOS_PER_NANOS (1000)
#endif
	/************/
/********************************/

/*******From Seconds*******/
#ifndef SECONDS_TO_MILLIS
#define SECONDS_TO_MILLIS(sec) (sec * MILLIS_PER_SECOND)
#endif
#ifndef SECONDS_TO_MICROS
#define SECONDS_TO_MICROS(sec) (sec * MICROS_PER_SECOND)
#endif
#ifndef SECONDS_TO_NANOS
#define SECONDS_TO_NANOS(sec) (sec * NANOS_PER_SECOND)
#endif
#ifndef SECONDS_TO_PICOS
#define SECONDS_TO_PICOS(sec) (sec * PICOS_PER_SECOND)
#endif
/**************************/

/*******From Millis*******/
#ifndef MILLIS_TO_SECONDS
#define MILLIS_TO_SECONDS(millis) (millis / MILLIS_PER_SECOND)
#endif
#ifndef MILLIS_TO_MICROS
#define MILLIS_TO_MICROS(millis) (millis * MICROS_PER_MILLIS)
#endif
#ifndef MILLIS_TO_NANOS
#define MILLIS_TO_NANOS(millis) (millis * NANOS_PER_MILLIS)
#endif
#ifndef MILLIS_TO_PICOS
#define MILLIS_TO_PICOS(millis) (millis * PICOS_PER_MILLIS)
#endif
/*************************/

/*******From Micros*******/
#ifndef MICROS_TO_SECONDS
#define MICROS_TO_SECONDS(micros) (micros / MICROS_PER_SECOND)
#endif
#ifndef MICROS_TO_MILLIS
#define MICROS_TO_MILLIS(micros) (micros / MICROS_PER_MILLIS)
#endif
#ifndef MICROS_TO_NANOS
#define MICROS_TO_NANOS(micros) (micros * NANOS_PER_MICROS)
#endif
#ifndef MICROS_TO_PICOS
#define MICROS_TO_PICOS(micros) (micros * PICOS_PER_MICROS)
#endif
/*************************/

/*******From Nanos*******/
#ifndef NANOS_TO_SECONDS
#define NANOS_TO_SECONDS(nanos) (nanos / NANOS_PER_SECOND)
#endif
#ifndef NANOS_TO_MILLIS
#define NANOS_TO_MILLIS(nanos) (nanos / NANOS_PER_MILLIS)
#endif
#ifndef NANOS_TO_MICROS
#define NANOS_TO_MICROS(nanos) (nanos / NANOS_PER_MICROS)
#endif
#ifndef NANOS_TO_PICOS
#define NANOS_TO_PICOS(nanos) (nanos * PICOS_PER_NANOS)
#endif
/************************/

/*******From Pico*******/
#ifndef PICOS_TO_SECONDS
#define PICOS_TO_SECONDS(picos) (picos / PICOS_PER_SECOND)
#endif
#ifndef PICOS_TO_MILLIS
#define PICOS_TO_MILLIS(picos) (picos / PICOS_PER_MILLIS)
#endif
#ifndef PICOS_TO_MICROS
#define PICOS_TO_MICROS(picos) (picos / PICOS_PER_MICROS)
#endif
#ifndef PICOS_TO_NANOS
#define PICOS_TO_NANOS(picos) (picos / PICOS_PER_NANOS)
#endif
/***********************/

#endif
