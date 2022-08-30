#include <iostream>
#include <pthread.h>
#include <cmath>
#include "header.h"

long double answer;
pthread_mutex_t mutex;

long double function_integral (long double x) {  // the function whose integral is calculated
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

void* integral(void* args) {  // a function, called on a thread
  Args* arg = reinterpret_cast<Args*> (args);
  long double sum = 0.l;
  switch (arg->st_method) {
    case 0:  // left rectangle method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x = arg->st_a + i * arg->st_step;
        sum += arg->st_func(x);
      }
      break;
    case 1:  // right rectangle method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x = arg->st_a + (i + 1) * arg->st_step;
        sum += arg->st_func(x);
      }
      break;
    case 2:  // middle rectangle method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x = arg->st_a + (static_cast<long double> (i) + 0.5l) * arg->st_step;
        sum += arg->st_func(x);
      }
      break;
    case 3:  // trapezoidal method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x_1 = arg->st_a + i * arg->st_step;
        long double x_2 = arg->st_a + (i + 1) * arg->st_step;
        sum += arg->st_func(x_1) + arg->st_func(x_2);
      }
      sum /= 2.l;
      break;
    case 4:  // parabola method (Simpson's method)
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x_left = arg->st_a + i * arg->st_step;
        long double x_middle = arg->st_a + (static_cast<long double> (i) + 0.5l) * arg->st_step;
        long double x_right = arg->st_a + (i + 1) * arg->st_step;
        sum += arg->st_func(x_left) + 4.l * arg->st_func(x_middle) + arg->st_func(x_right);
      }
      sum /= 6.l;
      break;
    default:
      std::cerr << "Undefined method\n";
      arg->st_error = true;
      return NULL;
  }
  // the beginning of the access to the critical section, i.e. to a variable 'answer'
  if (pthread_mutex_lock(&mutex) != 0) {
    std::cerr << "Failed to lock a mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  answer += sum * arg->st_step;
  // the end of the access to the critical section, i.e. to a variable 'answer'
  if (pthread_mutex_unlock(&mutex) != 0) {
    std::cerr << "Failed to unlock a mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  arg->st_error = false;  // if there were not any errors, indicator is false
  return NULL;
}
