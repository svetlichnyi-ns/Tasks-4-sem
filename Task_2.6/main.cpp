#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>
#include <random>
#include <vector>
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
  std::vector<std::thread> threads_1(NumOfThreads_1);  // create a vector of threads
  std::vector<Args_1> VectorOfStructures_1(NumOfThreads_1);  // create a vector of structures
  int NumOfSegmentsPerThread = NumOfSegments / NumOfThreads_1;
  long double a = -1.l, b = 1.l;  // set limits of integration
  long double step = (b - a) / NumOfSegments;
  for (int j = 0; j < NumOfThreads_1; j++) {
    VectorOfStructures_1[j].st_from = j * NumOfSegmentsPerThread;
    VectorOfStructures_1[j].st_to = (j + 1) * NumOfSegmentsPerThread;
    VectorOfStructures_1[j].st_a = a;
    VectorOfStructures_1[j].st_step = step;
    VectorOfStructures_1[j].st_func = function_integral;
    // creation of threads
    threads_1[j] = std::thread(one_dim_integral, &VectorOfStructures_1[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads_1; j++) {
    threads_1[j].join();
  }
  // integrate over the remaining segments (if NumOfSegments isn't evenly divisible by NumOfThreads_1)
  if (NumOfSegments % NumOfThreads_1 != 0) {
    Args_1 LastThread;  // a structure instance, created especially for the last thread
    LastThread.st_from = NumOfThreads_1 * NumOfSegmentsPerThread;
    LastThread.st_to = NumOfSegments;
    LastThread.st_a = a;
    LastThread.st_step = step;
    LastThread.st_func = function_integral;
    std::thread last_thread(one_dim_integral, &LastThread);
    last_thread.join();
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << answer << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

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
  int NumOfPointsPerThread = NumOfPoints / NumOfThreads_2;
  std::vector<std::thread> threads_2(NumOfThreads_2);
  std::vector<Args_2> VectorOfStructures_2(NumOfThreads_2);
  for (int j = 0; j < NumOfThreads_2; j++) {
    VectorOfStructures_2[j].st_from = j * NumOfPointsPerThread;
    VectorOfStructures_2[j].st_to = (j + 1) * NumOfPointsPerThread;
    VectorOfStructures_2[j].st_a = a;
    VectorOfStructures_2[j].st_b = b;
    VectorOfStructures_2[j].st_func = function_integral;
    threads_2[j] = std::thread(two_dim_integral, &VectorOfStructures_2[j]);
  }

  for (int j = 0; j < NumOfThreads_2; j++) {
    threads_2[j].join();
  }

  // scatter remaining points (if NumOfPoints isn't evenly divisible by NumOfThreads_2)
  if (NumOfPoints % NumOfThreads_2 != 0) {
    Args_2 RemainingPoints;
    RemainingPoints.st_from = NumOfThreads_2 * NumOfPointsPerThread;
    RemainingPoints.st_to = NumOfPoints;
    RemainingPoints.st_a = a;
    RemainingPoints.st_b = b;
    RemainingPoints.st_func = function_integral;
    std::thread remaining_points(two_dim_integral, &RemainingPoints);
    remaining_points.join();
  }

  end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(15) << (10.l * (b - a)) * (long double) count / (long double) NumOfPoints;
  std::cout << "\nIt took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n";
  return 0;
}