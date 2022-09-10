#include <iostream>
#include <iomanip>
#include <vector>
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
  std::vector<std::thread> threads(NumOfThreads);  // create a vector of threads
  std::vector<Args> VectorOfStructures(NumOfThreads);  // create a vector of structures
  int NumOfSegmentsPerThread = NumOfSegments / NumOfThreads;
  long double a = -1.l, b = 1.l;  // set limits of integration
  long double step = (b - a) / NumOfSegments;
  for (int j = 0; j < NumOfThreads; j++) {
    VectorOfStructures[j].st_from = j * NumOfSegmentsPerThread;
    VectorOfStructures[j].st_to = (j + 1) * NumOfSegmentsPerThread;
    VectorOfStructures[j].st_a = a;
    VectorOfStructures[j].st_step = step;
    VectorOfStructures[j].st_func = function_integral;
    VectorOfStructures[j].st_method = static_cast<int>(method);
    // creation of threads
    threads[j] = std::thread(integral, &VectorOfStructures[j]);
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
  return 0;
}