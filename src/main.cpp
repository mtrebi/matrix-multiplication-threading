#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <windows.h>


static const long MATRIX_SIZE = 1000;
static const int THREADS_NUMBER = 4;
static const long N_EXECUTIONS = 1e3;

struct Matrix {
  float ** elements;

  void initialize_zero() {
    elements = new float*[MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      elements[i] = new float[MATRIX_SIZE];
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        elements[i][j] = 0.0f;
      }
    }
  }

  void initialize_random() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-1e9, -1e9);
    auto random = std::bind(dist, mt);
    elements = new float*[MATRIX_SIZE];
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      elements[i] = new float[MATRIX_SIZE];
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        elements[i][j] = random();
      }
    }
  }

  void print() {
    std::cout << std::endl;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      std::cout << "|\t";

      for (int j = 0; j < MATRIX_SIZE; ++j) {
        std::cout << elements[i][j] << "\t";
      }
      std::cout << "|" << std::endl;
    }
  }

};

void multiply(Matrix& r, const Matrix& m1, const Matrix& m2);
void single_execution(Matrix& r, long long& elapsed_time, const Matrix& m1, const Matrix& m2);
void multithreading_execution(Matrix& r, long long& elapsed_time, const Matrix& m1, const Matrix& m2);
void multiply_threading(Matrix& result, const int thread_number, const Matrix& m1, const Matrix& m2);
void benchmark_execution(void(*execution_function)(Matrix& r, long long& elapsed_time, const Matrix& m1, const Matrix& m2));
long long milliseconds_now();

int main() {
  std::cout << "Single execution" << std::endl;
  benchmark_execution(single_execution);
  std::cout << "Multi thread execution" << std::endl;
  benchmark_execution(multithreading_execution);
  Sleep(100000);
  std::cout << "End of program" << std::endl;
}

void benchmark_execution(void(*execution_function)(Matrix& r, long long& elapsed_time, const Matrix& m1, const Matrix& m2)) {
  Matrix m1, m2, r;

  long long total_time = 0.0;
  for (int i = 0; i < N_EXECUTIONS; ++i) {
    long long elapsed_time = 0.0;
    m1.initialize_random();
    m2.initialize_random();
    r.initialize_zero();

    execution_function(r, elapsed_time, m1, m2);
    total_time += elapsed_time;
  }
  std::cout << "\tAverage execution took\t" << (double) total_time / N_EXECUTIONS << " ms" << std::endl;
}

void multiply(Matrix& r, const Matrix& m1, const Matrix& m2) {
  for (int i = 0; i < MATRIX_SIZE; ++i) {
    for (int j = 0; j < MATRIX_SIZE; ++j) {
      float result = 0.0f;
      for (int k = 0; k < MATRIX_SIZE; ++k) {
        const float e1 = m1.elements[i][k];
        const float e2 = m2.elements[k][j];
        result += e1 * e2;
      }
      r.elements[i][j] = result;
    }
  }
}

void single_execution(Matrix& r, long long& elapsed_time, const Matrix& m1, const Matrix& m2) {
  //std::cout << "Starting single thread execution..." << std::endl;
  long long start_time = milliseconds_now();

  //std::cout << "Calculating...." << std::endl;
  multiply(r, m1, m2);

  long long end_time = milliseconds_now();
  //std::cout << "Finishing single thread execution..." << std::endl;

  elapsed_time = end_time - start_time;
}

void multiply_threading(Matrix& result, const int thread_number, const Matrix& m1, const Matrix& m2) {
  // Calculate workload
  const int n_elements = (MATRIX_SIZE * MATRIX_SIZE);
  const int n_operations = n_elements / THREADS_NUMBER;
  const int rest_operations = n_elements % THREADS_NUMBER;

  int start_op, end_op;

  if (thread_number == 0) {
    // First thread does more job
    start_op = n_operations * thread_number;
    end_op = (n_operations * (thread_number + 1)) + rest_operations;
  }
  else {
    start_op = n_operations * thread_number + rest_operations;
    end_op = (n_operations * (thread_number + 1)) + rest_operations;
  }

  for (int op = start_op; op < end_op; ++op) {
    const int row = op % MATRIX_SIZE;
    const int col = op / MATRIX_SIZE;
    float r = 0.0f;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      const float e1 = m1.elements[row][i];
      const float e2 = m2.elements[i][col];
      r += e1 * e2;
    }

    result.elements[row][col] = r;
  }
}

void multithreading_execution(Matrix& r, long long& elapsed_time, const Matrix& m1, const Matrix& m2) {
  //std::cout << "Starting multithreading execution..." << std::endl;
  long long start_time = milliseconds_now();

  std::thread threads[THREADS_NUMBER];

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    //std::cout << "Starting thread " << i << std::endl;
    threads[i] = std::thread(multiply_threading, std::ref(r), i, std::ref(m1), std::ref(m2));
  }

  //std::cout << "Calculating...." << std::endl;

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    //std::cout << "Joining thread " << i << std::endl;
    threads[i].join();
  }

  long long end_time = milliseconds_now();
  //std::cout << "Finishing multithreading execution..." << std::endl;

  elapsed_time = end_time - start_time;
}

long long milliseconds_now() {
  static LARGE_INTEGER s_frequency;
  static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
  if (s_use_qpc) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (1000LL * now.QuadPart) / s_frequency.QuadPart;
  }
  else {
    return GetTickCount();
  }
}