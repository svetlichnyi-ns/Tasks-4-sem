#ifndef HEADER_H
#define HEADER_H
#include <pthread.h>

extern long double answer;  // a global variable, common for all threads
extern pthread_mutex_t mutex;  // it's used for threads' synchronization

typedef struct {  // the arguments of a function, called on a thread, are "wrapped" in a structure
  int st_from, st_to;  // initial and final segments of integration for a given thread
  long double st_a;  // a left limit of integration
  long double st_step;  // the length of each segment of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
  int st_method;  // number of method, which is used for integral's calculation
} Args;

long double function_integral (long double x);  // the function whose integral is calculated
void* integral(void* args);  // a function, called on a thread

#endif