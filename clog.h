#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#if defined(__APPLE__) && defined(__MACH__) || defined (__linux__) //TODO add Unix and Solaris
#include <unistd.h>
#include <pthread.h>
#define SEPARATOR ('/')
#elif _WIN32
#include <Windows.h>
#define SEPARATOR ('\\')
#else
#error Platform not supprted
#endif

#ifdef ENABLE_LOG_COLOR
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#else
#define COLOR_RED     ""
#define COLOR_GREEN   ""
#define COLOR_YELLOW  ""
#define COLOR_BLUE    ""
#define COLOR_MAGENTA ""
#define COLOR_CYAN    ""
#define COLOR_RESET   ""
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 256
#endif


#ifdef ENABLE_LOG

int n = 2;
static int *writefd = &n; //for different socket support

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__) //TODO siblings
pthread_mutex_t _lock;
#else
//windows locks
#endif
const int init_clog() {
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_init(&_lock, NULL);
#else
	//windows mutex init
#endif
	return 0;
}
#endif

/** Changes the default printing socket
 * @param sockfd The socket descriptor to write to
 */
void change_out_socket(int *sockfd) {
#ifdef ENABLE_LOG
	writefd = sockfd;
#endif
}

void log_inf(const char *tag, const char *msg, ...) {
#ifdef ENABLE_LOG
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_lock(&_lock);
#endif
	char *str = (char *)malloc(BUFFER_SIZE);
	va_list vl;
	va_start(vl, msg);
	vsprintf(str, msg, vl);
	va_end(vl);
	printf("%s[!]%s %s: %s\n", COLOR_YELLOW, COLOR_RESET, tag, str);
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_unlock(&_lock);
#endif
#endif
}

void log_err(const char *tag, const char *msg, ...) {
#ifdef ENABLE_LOG
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_lock(&_lock);
#endif
	char *str = (char *)malloc(BUFFER_SIZE);
	va_list vl;
	va_start(vl, msg);
	vsprintf(str, msg, vl);
	va_end(vl);
	printf("[X] %s: %s\n", tag, str);
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_unlock(&_lock);
#endif
#endif
}

void log_per(const char *tag, const char *msg, ...) {
#ifdef ENABLE_LOG
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_lock(&_lock);
#endif
	char *str = (char *)malloc(BUFFER_SIZE);
	va_list vl;
	va_start(vl, msg);
	vsprintf(str, msg, vl);
	va_end(vl);
	perror(str);
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_unlock(&_lock);
#endif
#endif
}

void log_fat(const char *tag, const char *msg, ...) {
#ifdef ENABLE_LOG
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_lock(&_lock);
#endif
	char *str = (char *)malloc(BUFFER_SIZE);
	va_list vl;
	va_start(vl, msg);
	vsprintf(str, msg, vl);
	va_end(vl);
	printf("[FATAL] %s: %s\n", tag, str);
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	pthread_mutex_unlock(&_lock);
#endif
#endif
}
#endif
