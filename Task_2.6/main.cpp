#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <random>
#include "header.h"

int main() {
  std::cout << "One-dimensional Monte Carlo method\n";
  std::cout << "Enter the number of threads for the first task: ";
  int NumOfThreads_1;
  std::cin >> NumOfThreads_1;
  if (NumOfThreads_1 < 1) {
    std::cerr << "The number of threads must be positive\n";
    return -1;
  }

  std::cout << "Enter the number of segments of integration: ";
  int NumOfSegments;
  std::cin >> NumOfSegments;
  if (NumOfSegments < 1) {
    std::cout << "The number of segments of integration must be positive\n";
    return -1;
  }

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  answer = 0.l;  // set an initial value of the integral
  // create an array of threads' identificators
  pthread_t* threads_1 = (pthread_t*) malloc ((NumOfThreads_1) * sizeof(pthread_t));
  if (threads_1 == NULL) {
    std::cerr << "Failed to allocate memory for an array of threads' identificators via malloc()\n";
    return -1;
  }
  // create an array of structures
  Args_1* ArrayOfStructures_1 = (Args_1*) malloc ((NumOfThreads_1) * sizeof(Args_1));
  if (ArrayOfStructures_1 == NULL) {
    std::cerr << "Failed to allocate memory for an array of structures via malloc()\n";
    free(threads_1);
    return -1;
  }
  // the initialization of the first mutex
  if (pthread_mutex_init(&mutex_1, NULL) != 0) {
    std::cerr << "Failed to initialize the first mutex!\n";
    free(threads_1);
    free(ArrayOfStructures_1);
    return -1;
  }

  int NumOfSegmentsPerThread = NumOfSegments / NumOfThreads_1;
  long double a = -1.l, b = 1.l;  // set limits of integration
  long double step = (b - a) / NumOfSegments;
  for (int j = 0; j < NumOfThreads_1; j++) {
    ArrayOfStructures_1[j].st_from = j * NumOfSegmentsPerThread;
    ArrayOfStructures_1[j].st_to = (j + 1) * NumOfSegmentsPerThread;
    ArrayOfStructures_1[j].st_a = a;
    ArrayOfStructures_1[j].st_step = step;
    ArrayOfStructures_1[j].st_func = function_integral;
    // creation of threads
    if (pthread_create(&threads_1[j], NULL, one_dim_integral, &ArrayOfStructures_1[j]) != 0) {
      std::cerr << "Failed to create a thread!\n";
      free(threads_1);
      free(ArrayOfStructures_1);
      return -1;
    }
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads_1; j++) {
    if (pthread_join(threads_1[j], NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads_1);
      free(ArrayOfStructures_1);
      return -1;
    }
  }
  // integrate over the remaining segments (if NumOfSegments isn't evenly divisible by NumOfThreads_1)
  if (NumOfSegments % NumOfThreads_1 != 0) {
    pthread_t last_thread;  // an identificator of the last thread
    Args_1 LastThread;  // a structure instance, created especially for the last thread
    LastThread.st_from = NumOfThreads_1 * NumOfSegmentsPerThread;
    LastThread.st_to = NumOfSegments;
    LastThread.st_a = a;
    LastThread.st_step = step;
    LastThread.st_func = function_integral;
    if (pthread_create(&last_thread, NULL, one_dim_integral, &LastThread) != 0) {
      std::cerr << "Failed to create a thread!\n";
      free(threads_1);
      free(ArrayOfStructures_1);
      return -1;
    }
    if (pthread_join(last_thread, NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads_1);
      free(ArrayOfStructures_1);
      return -1;
    }
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << answer << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

  // the destroyment of the first mutex
  if (pthread_mutex_destroy(&mutex_1) != 0) {
    std::cerr << "Failed to destroy the first mutex!\n";
    free(threads_1);
    free(ArrayOfStructures_1);
    return -1;
  }
  free(threads_1);
  free(ArrayOfStructures_1);

  std::cout << "Two-dimensional Monte Carlo method\n";
  std::cout << "Enter the number of threads for the second task: ";
  int NumOfThreads_2;
  std::cin >> NumOfThreads_2;
  if (NumOfThreads_2 < 1) {
    std::cerr << "The number of threads must be positive\n";
    return -1;
  }

  int NumOfPoints;
  std::cout << "Enter the number of points you want to scatter: ";
  std::cin >> NumOfPoints;
  if (NumOfPoints < 1) {
    std::cerr << "The number of points must be positive\n";
    return -1;
  }

  begin = std::chrono::steady_clock::now();  // start the timer
  count = 0;  // set an initial value of 'count'
  // the initialization of the second mutex
  if (pthread_mutex_init(&mutex_2, NULL) != 0) {
    std::cerr << "Failed to initialize the second mutex!\n";
    return -1;
  }
  
  int NumOfPointsPerThread = NumOfPoints / NumOfThreads_2;
  pthread_t* threads_2 = (pthread_t*) malloc (NumOfThreads_2 * sizeof(pthread_t));
  if (threads_2 == NULL) {
    std::cerr << "Failed to allocate memory for an array of threads' identificators via malloc()\n";
    return -1;
  }
  Args_2* ArrayOfStructures_2 = (Args_2*) malloc (NumOfThreads_2 * sizeof(Args_2));
  if (ArrayOfStructures_2 == NULL) {
    std::cerr << "Failed to allocate memory for an array of structures via malloc()\n";
    free(threads_2);
    return -1;
  }
  for (int j = 0; j < NumOfThreads_2; j++) {
    ArrayOfStructures_2[j].st_from = j * NumOfPointsPerThread;
    ArrayOfStructures_2[j].st_to = (j + 1) * NumOfPointsPerThread;
    ArrayOfStructures_2[j].st_a = a;
    ArrayOfStructures_2[j].st_b = b;
    ArrayOfStructures_2[j].st_func = function_integral;
    if (pthread_create(&threads_2[j], NULL, two_dim_integral, &ArrayOfStructures_2[j]) != 0) {
      std::cerr << "Failed to create a thread!\n";
      free(threads_2);
      free(ArrayOfStructures_2);
      return -1;
    }
  }

  for (int j = 0; j < NumOfThreads_2; j++) {
    if (pthread_join(threads_2[j], NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads_2);
      free(ArrayOfStructures_2);
      return -1;
    }
  }

  // scatter remaining points (if NumOfPoints isn't evenly divisible by NumOfThreads_2)
  if (NumOfPoints % NumOfThreads_2 != 0) {
    pthread_t remaining_points;
    Args_2 RemainingPoints;
    RemainingPoints.st_from = NumOfThreads_2 * NumOfPointsPerThread;
    RemainingPoints.st_to = NumOfPoints;
    RemainingPoints.st_a = a;
    RemainingPoints.st_b = b;
    RemainingPoints.st_func = function_integral;
    if (pthread_create(&remaining_points, NULL, two_dim_integral, &RemainingPoints) != 0) {
      std::cerr << "Failed to create a thread!\n";
      free(threads_2);
      free(ArrayOfStructures_2);
      return -1;
    }
    if (pthread_join(remaining_points, NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads_2);
      free(ArrayOfStructures_2);
      return -1;
    }
  }

  end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(15) << (10.l * (b - a)) * (long double) count / (long double) NumOfPoints;
  std::cout << "\nIt took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n";
  // the destroyment of the second mutex
  if (pthread_mutex_destroy(&mutex_2) != 0) {
    std::cerr << "Failed to destroy the second mutex!\n";
    free(threads_2);
    free(ArrayOfStructures_2);
    return -1;
  }
  // everything was successful; all we have to do is to release memory
  free(threads_2);
  free(ArrayOfStructures_2);
  return 0;
}