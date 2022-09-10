#include <iostream>
#include <thread>
#include <random>
#include <mutex>
#include <cmath>
#include "header.h"

long double answer = 0.l;
int count = 0;
std::mutex mutex_1;
std::mutex mutex_2;

long double function_integral (long double x) {  // the function whose integral is calculated
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

void one_dim_integral(void* args) {
  Args_1* arg = reinterpret_cast<Args_1*> (args);
  long double sum = 0.l;
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  for (int i = arg->st_from; i < arg->st_to; i++) {
    // on each interval of integration, a thread chooses an arbitrary abscissa
    long double x_random = arg->st_a + i * arg->st_step + arg->st_step * distr(eng);
    sum += arg->st_func(x_random);
  }
  // the beginning of the access to the critical section, i.e. to a variable 'answer'
  mutex_1.lock();
  answer += sum * arg->st_step;
  // the end of the access to the critical section, i.e. to a variable 'answer'
  mutex_1.unlock();
  return;
}

void two_dim_integral(void* args) {
  Args_2* arg = reinterpret_cast<Args_2*> (args);
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  // the beginning of the access to the critical section, i.e. to a variable 'count'
  mutex_2.lock();
  for (int j = arg->st_from; j < arg->st_to; j++) {
    // a random abscissa, belonging to the segment [a; b]
    long double x = arg->st_a + (arg->st_b - arg->st_a) * distr(eng);
    // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
    long double y = 10.l * distr(eng);
    if (y <= arg->st_func(x))
      count++;
  }
  // the end of the access to the critical section, i.e. to a variable 'count'
  mutex_2.unlock();
  return;
}