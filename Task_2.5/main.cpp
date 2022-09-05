#include <iostream>
#include <iomanip>
#include <new>
#include <chrono>
#include <cmath>
#include <thread>
#include <mutex>
#include "header.h"

int main() {
  std::cout << "Enter the number of threads: ";
  int NumOfThreads;
  std::cin >> NumOfThreads;
  if (NumOfThreads < 1) {
    std::cerr << "The number of threads must be positive\n";
    return -1;
  }

  std::cout << "Enter the number of segments of integration: ";
  int NumOfSegments;
  std::cin >> NumOfSegments;
  if (NumOfSegments < 1) {
    std::cerr << "The number of segments of integration must be positive\n";
    return -1;
  }

  std::cout << "Enter the number of method that you would like to use for integral's calculation:\n";
  std::cout << "0 - left rectangle method;\n";
  std::cout << "1 - right rectangle method;\n";
  std::cout << "2 - middle rectangle method;\n";
  std::cout << "3 - trapezoidal method;\n";
  std::cout << "4 - parabola method (Simpson's method).\n";
  long double method;
  std::cin >> method;
  if ((method < 0.l) || (method > 4.l)) {
    std::cerr << "Use one of available methods.\n";
    return -1;
  }
  if ((method - static_cast<int>(method) > 0.l) || (method - static_cast<int>(method) < 0.l)) {
    std::cerr << "Undefined method.\n";
    return -1;
  }

  // start the timer
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  answer = 0.l;  // set an initial value of the integral
  // create an array of threads
  std::thread* threads;
  try {
    threads = new std::thread [NumOfThreads];
  }
  catch (const std::bad_alloc& e) {
    std::cerr << "Failed to allocate memory for an array of threads: " << e.what() << std::endl;
    return -1;
  }
  // create an array of structures
  Args* ArrayOfStructures;
  try {
    ArrayOfStructures = new Args [NumOfThreads];
  }
  catch (const std::bad_alloc& e) {
    std::cerr << "Failed to allocate memory for an array of structures: " << e.what() << std::endl;
    delete [] threads;
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
    threads[j] = std::thread(integral, &ArrayOfStructures[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++) {
    threads[j].join();
  }
  // integrate over the remaining segments (if NumOfSegments isn't evenly divisible by NumOfThreads)
  if (NumOfSegments % NumOfThreads != 0) {
    Args LastThread;  // a structure instance, created especially for the last thread 
    LastThread.st_from = NumOfThreads * NumOfSegmentsPerThread;
    LastThread.st_to = NumOfSegments;
    LastThread.st_a = a;
    LastThread.st_step = step;
    LastThread.st_func = function_integral;
    LastThread.st_method = static_cast<int>(method);
    std::thread last_thread(integral, &LastThread);
    last_thread.join();
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << answer << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n";
  delete [] threads;
  delete [] ArrayOfStructures;
  return 0;
}