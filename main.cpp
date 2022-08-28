#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <assert.h>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include "header.h"

int main() {
  std::cout << "Enter the number of threads: ";
  int NumOfThreads;
  std::cin >> NumOfThreads;
  assert(NumOfThreads >= 1 && "The number of threads must be positive");

  std::cout << "Enter the number of segments of integration: ";
  int NumOfSegments;
  std::cin >> NumOfSegments;
  assert(NumOfSegments >= 1 && "The number of segments of integration must be positive");

  std::cout << "Enter the number of method that you would like to use for integral's calculation:\n";
  std::cout << "0 - left rectangle method;\n";
  std::cout << "1 - right rectangle method;\n";
  std::cout << "2 - middle rectangle method;\n";
  std::cout << "3 - trapezoidal method;\n";
  std::cout << "4 - parabola method (Simpson's method).\n";
  long double method;
  std::cin >> method;
  assert(method >= 0.l && method <= 4.l && "Use one of available methods");
  if (method - static_cast<int>(method) != 0.l) {
    std::cerr << "Undefined method\n";
    return -1;
  }

  // start the timer
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  pi = 0.l;  // set an initial value of PI
  // create an array of threads' identificators
  pthread_t* threads = (pthread_t*) malloc ((NumOfThreads) * sizeof(pthread_t));
  if (threads == NULL) {
    std::cerr << "Failed to allocate memory for an array of threads' identificators via malloc()\n";
    return -1;
  }
  // create an array of structures
  Args* ArrayOfStructures = (Args*) malloc ((NumOfThreads) * sizeof(Args));
  if (ArrayOfStructures == NULL) {
    std::cerr << "Failed to allocate memory for an array of structs via malloc()\n";
    free(threads);
    return -1;
  }
  // the initialization of the mutex
  if (pthread_mutex_init(&mutex, NULL) != 0) {
    std::cerr << "Failed to initialize a mutex!\n";
    free(threads);
    free(ArrayOfStructures);
    return -1;
  }

  int NumOfSegmentsPerThread = NumOfSegments / NumOfThreads;
  long double a = -1.l, b = 1.l;  // set limits of integration
  long double step = (b - a) / NumOfSegments;
  for (int j = 0; j < NumOfThreads; j++) {
    ArrayOfStructures[j].st_from = j * NumOfSegmentsPerThread;
    ArrayOfStructures[j].st_to = (j + 1) * NumOfSegmentsPerThread;
    ArrayOfStructures[j].st_a = a;
    ArrayOfStructures[j].st_step = step;
    ArrayOfStructures[j].st_func = function_integral;
    ArrayOfStructures[j].st_method = static_cast<int>(method);
    // creation of threads
    if (pthread_create(&threads[j], NULL, integral, &ArrayOfStructures[j]) != 0) {
      std::cerr << "Failed to create a thread!\n";
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
    // checking whether an error occurred within a function
    if (ArrayOfStructures[j].st_error == true) {
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++) {
    if (pthread_join(threads[j], NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
  }
  // integrate over the remaining segments (if NumOfSegments isn't evenly divisible by NumOfThreads)
  if (NumOfSegments % NumOfThreads != 0) {
    pthread_t last_thread;  // an identificator of the last thread
    Args LastThread;  // a structure instance, created especially for the last thread 
    LastThread.st_from = NumOfThreads * NumOfSegmentsPerThread;
    LastThread.st_to = NumOfSegments;
    LastThread.st_a = a;
    LastThread.st_step = step;
    LastThread.st_func = function_integral;
    LastThread.st_method = static_cast<int>(method);
    if (pthread_create(&last_thread, NULL, integral, &LastThread) != 0) {
      std::cerr << "Failed to create a thread!\n";
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
    if (LastThread.st_error == true) {
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
    if (pthread_join(last_thread, NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI is equal to: " << std::setprecision(20) << pi << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate PI\n";
  // the destroyment of the mutex
  if (pthread_mutex_destroy(&mutex) != 0) {
    std::cerr << "Failed to destroy a mutex!\n";
    free(threads);
    free(ArrayOfStructures);
    return -1;
  }
  free(threads);
  free(ArrayOfStructures);
  return 0;
}