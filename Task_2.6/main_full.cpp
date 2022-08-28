#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <assert.h>
#include <cstdlib>
#include <chrono>
#include <cmath>

long double function_integral (long double x) {  // the function whose integral is calculated
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

long double answer = 0.l;  // a global variable, used in one-dimensional Monte Carlo method
int count = 0;  // a global variable, used in two-dimensional Monte Carlo method
pthread_mutex_t mutex_1;
pthread_mutex_t mutex_2;

typedef struct {
  int st_from, st_to;  // initial and final segments of integration for a given thread
  long double st_a;  // a left limit of integration
  long double st_step;  // the length of each segment of integration
  bool st_error;  // an indicator of an error inside a function, called on a thread
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_1;

typedef struct {
  int st_from, st_to;  // initial and final numbers of points, scattered by a given thread
  long double st_a, st_b;  // left and right limits of integration
  bool st_error;  // an indicator of an error inside a function, called on a thread
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_2;

void* one_dim_integral(void* args) {
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

void* two_dim_integral(void* args) {
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

int main() {
  std::cout << "One-dimensional Monte Carlo method\n";
  std::cout << "Enter the number of threads for the first task: ";
  int NumOfThreads_1;
  std::cin >> NumOfThreads_1;
  assert(NumOfThreads_1 >= 1 && "The number of threads must be positive");

  std::cout << "Enter the number of segments of integration: ";
  int NumOfSegments;
  std::cin >> NumOfSegments;
  assert(NumOfSegments >= 1 && "The number of segments of integration must be positive");

  // start the timer
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
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
    // checking whether an error occurred within a function
    if (ArrayOfStructures_1[j].st_error == true) {
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
    if (LastThread.st_error == true) {
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
  assert(NumOfThreads_2 >= 1 && "The number of threads must be positive");

  int NumOfPoints;
  std::cout << "Enter the number of points you want to scatter: ";
  std::cin >> NumOfPoints;
  assert(NumOfPoints >= 1 && "The number of points must be positive");

  begin = std::chrono::steady_clock::now();  // start the timer
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
    // checking whether an error occurred within a function
    if (ArrayOfStructures_2[j].st_error == true) {
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
    if (RemainingPoints.st_error == true) {
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
  free(threads_2);
  free(ArrayOfStructures_2);
  return 0;
}