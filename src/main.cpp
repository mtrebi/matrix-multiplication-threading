#include <iostream>
#include <random>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

// Definitions

static const long MATRIX_SIZE = 1000;
static const int THREADS_NUMBER = 1;

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
    std::default_random_engine generator(time(NULL));
    std::uniform_int_distribution<int> distribution(-1e9, 1e9);
    auto random = std::bind(distribution, generator);
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

// Forward declaration
Matrix multiply(const Matrix& m1, const Matrix& m2);
void mainthread_execution(Matrix& r, const Matrix& m1, const Matrix& m2);
void multiply_threading(Matrix& result, const int thread_number, const Matrix& m1, const Matrix& m2);
void multithreading_execution(Matrix& r, const Matrix& m1, const Matrix& m2);
const double elapsed_time(timespec& m_start, timespec& m_end);

int main() {
  // Initialize threads
  Matrix m1, m2, r;
  m1.initialize_random();
  m2.initialize_random();
  r.initialize_zero();

  //mainthread_execution(r, m1, m2);
  multithreading_execution(r, m1, m2);
  Sleep(100000);
}

Matrix multiply(const Matrix& m1, const Matrix& m2) {
  Matrix r;
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
  return r;
}

void mainthread_execution(Matrix& r, const Matrix& m1, const Matrix& m2) {
  std::cout << "Starting main thread execution..." << std::endl;
  // TODO: Set start timer

  std::cout << "Calculating...." << std::endl;
  r = multiply(m1, m2);

  // TODO: Set end timer
  std::cout << "Finishing multithreading execution..." << std::endl;
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

void multithreading_execution(Matrix& r, const Matrix& m1, const Matrix& m2) {
  std::cout << "Starting multithreading execution..." << std::endl;
  // TODO: Set start timer

  std::thread threads[THREADS_NUMBER];

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    std::cout << "Starting thread " << i << std::endl;
    threads[i] = std::thread(multiply_threading, std::ref(r), i, m1, m2);
  }

  std::cout << "Calculating...." << std::endl;

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    std::cout << "Joining thread " << i << std::endl;
    threads[i].join();
  }

  // TODO: Set end timer
  std::cout << "Finishing multithreading execution..." << std::endl;
}

const double elapsed_time(timespec& m_start, timespec& m_end) {
  timespec temp;
  if ((m_end.tv_nsec - m_start.tv_nsec) < 0) {
    temp.tv_sec = m_end.tv_sec - m_start.tv_sec - 1;
    temp.tv_nsec = 1e9 + m_end.tv_nsec - m_start.tv_nsec;
  }
  else {
    temp.tv_sec = m_end.tv_sec - m_start.tv_sec;
    temp.tv_nsec = m_end.tv_nsec - m_start.tv_nsec;
  }

  const double time_sec = (double)temp.tv_sec;
  const double time_nsec = (double)temp.tv_nsec;
  const double time_msec = (time_sec * 1e3) + (time_nsec / 1e6);

  return time_msec;
}
