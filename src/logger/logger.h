#ifndef LOGGER_H
#define LOGGER_H 

#include <stdio.h>
#include <time.h>

#define LOG_TIME_FORMAT "%H:%M:%S %d-%m-%y"

#ifdef DEBUG_LOG
	#define debug_log(...)                                  \
		do {                                                \
			time_t timer;                                   \
			char buffer[32];                                \
			struct tm *tm_info;                             \
			time(&timer);                                   \
			tm_info = localtime(&timer);                    \
			strftime(buffer, 32, LOG_TIME_FORMAT, tm_info); \
			fprintf(stdout, "%s: ", buffer);                \
			fprintf(stdout, __VA_ARGS__);                   \
			fprintf(stdout, "\n");                          \
		} while (0)
#else 
	#define debug_log(...) do {} while (0)
#endif // DEBUG_LOG

#ifdef PROD_LOG
	#define prod_log(...)                                   \
		do {                                                \
			time_t timer;                                   \
			char buffer[32];                                \
			struct tm *tm_info;                             \
			time(&timer);                                   \
			tm_info = localtime(&timer);                    \
			strftime(buffer, 32, LOG_TIME_FORMAT, tm_info); \
			fprintf(stdout, "%s: ", buffer);                \
			fprintf(stdout, __VA_ARGS__);                   \
			fprintf(stdout, "\n");                          \
		} while (0)
#else 
	#define prod_log(...) do {} while (0)
#endif // PROD_LOG

#ifdef ERROR_LOG
	#define error_log(...)                                  \
		do {                                                \
			time_t timer;                                   \
			char buffer[32];                                \
			struct tm *tm_info;                             \
			time(&timer);                                   \
			tm_info = localtime(&timer);                    \
			strftime(buffer, 32, LOG_TIME_FORMAT, tm_info); \
			fprintf(stderr, "%s: ", buffer);                \
			fprintf(stderr, __VA_ARGS__);                   \
			fprintf(stderr, "\n");                          \
		} while (0)
#else 
	#define error_log(...) do {} while (0)
#endif // ERROR_LOG

#endif // LOGGER_H
