#ifndef HEADER_H
#define HEADER_H
#include <mutex>

extern long double answer;  // a global variable, used in one-dimensional Monte Carlo method
extern int count;  // a global variable, used in two-dimensional Monte Carlo method
extern std::mutex mutex_1;  // it's used for synchronization of threads, working on the first task
extern std::mutex mutex_2;  // it's used for synchronization of threads, working on the second task

typedef struct {
  int st_from, st_to;  // initial and final segments of integration for a given thread
  long double st_a;  // a left limit of integration
  long double st_step;  // the length of each segment of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_1;

typedef struct {
  int st_from, st_to;  // initial and final numbers of points, scattered by a given thread
  long double st_a, st_b;  // left and right limits of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_2;

long double function_integral (long double x);  // the function whose integral is calculated
void one_dim_integral(void* args);  // a function, called on a thread, working on the first task
void two_dim_integral(void* args);  // a function, called on a thread, working on the second task

#endif