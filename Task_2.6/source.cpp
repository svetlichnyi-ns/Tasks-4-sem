#include <iostream>
#include <pthread.h>
#include <cmath>
#include "header.h"

long double function_integral (long double x) {  // the function whose integral is calculated
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

void* one_dim_integral(void* args) {  // a function, called on a thread, working on the first task
  Args_1* arg = reinterpret_cast<Args_1*> (args);
  long double sum = 0.l;
  for (int i = arg->st_from; i < arg->st_to; i++) {
    // on each interval of integration, a thread chooses an arbitrary abscissa
    long double x_random = arg->st_a + i * arg->st_step + arg->st_step * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
    sum += arg->st_func(x_random);
  }
  // the beginning of the access to the critical section, i.e. to a variable 'answer'
  if (pthread_mutex_lock(&mutex_1) != 0) {
    std::cerr << "Failed to lock the first mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  answer += sum * arg->st_step;
  // the end of the access to the critical section, i.e. to a variable 'answer'
  if (pthread_mutex_unlock(&mutex_1) != 0) {
    std::cerr << "Failed to unlock the first mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  arg->st_error = false;  // if there were not any errors, indicator is false
  return NULL;
}

void* two_dim_integral(void* args) {  // a function, called on a thread, working on the second task
  Args_2* arg = reinterpret_cast<Args_2*> (args);
  // the beginning of the access to the critical section, i.e. to a variable 'count'
  if (pthread_mutex_lock(&mutex_2) != 0) {
    std::cerr << "Failed to lock the second mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  for (int j = arg->st_from; j < arg->st_to; j++) {
    // a random abscissa, belonging to the segment [a; b]
    long double x = arg->st_a + (arg->st_b - arg->st_a) * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
    // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
    long double y = 10.l * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
    if (y <= arg->st_func(x))
      count++;
  }
  // the end of the access to the critical section, i.e. to a variable 'count'
  if (pthread_mutex_unlock(&mutex_2) != 0) {
    std::cerr << "Failed to unlock the second mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  arg->st_error = false;  // if there were not any errors, indicator is false
  return NULL;
}