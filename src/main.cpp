#include <iostream>
#include <random>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>


static const int MATRIX_SIZE = 3;
static const int THREADS_NUMBER = 1;
//unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

struct Matrix {
  float elements [MATRIX_SIZE][MATRIX_SIZE];

  void initialize_zero() {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        elements[i][j] = 0.0f;
      }
    }
  }

  void initialize_random() {
    srand(time(NULL));
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        elements[i][j] = rand();
      }
    }
  }

  void initialize_test() {
    elements[0][0] = 1;
    elements[0][1] = 2;
    elements[0][2] = 3;
    elements[1][0] = 4;
    elements[1][1] = 5;
    elements[1][2] = 6;
    elements[2][0] = 7;
    elements[2][1] = 8;
    elements[2][2] = 9;
  }

  Matrix multiply(const Matrix& m) {
    Matrix r;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      for (int j = 0; j < MATRIX_SIZE; ++j) {
        float result = 0.0f;
        for (int k = 0; k < MATRIX_SIZE; ++k) {
          const float e1 = elements[i][k];
          const float e2 = m.elements[k][j];
          result += e1 * e2;
        }
        r.elements[i][j] = result;
      }
    }
    return r;
  }

  void print() {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      std::cout << "|\t";

      for (int j = 0; j < MATRIX_SIZE; ++j) {
        std::cout << elements[i][j] << "\t";
      }
      std::cout << "|" << std::endl;
    }
  }

};


void a() {
  Matrix m1, m2;
  m1.initialize_test();
  m2.initialize_test();


  Matrix r = m1.multiply(m2);
  r.print();


}

void multiply_threading(Matrix& result, const int thread_number, const Matrix& m1, const Matrix& m2){
  // Calculate workload
  const int n_elements = (MATRIX_SIZE * MATRIX_SIZE);

  const int n_operations = n_elements / THREADS_NUMBER;
  const int start_op = n_operations * thread_number;
  const int end_op = (n_operations * (thread_number + 1)) - 1;

  const int start_row = start_op % MATRIX_SIZE;
  const int start_col = start_op / MATRIX_SIZE;

  const int end_row = end_op % MATRIX_SIZE + 1;
  const int end_col = end_op / MATRIX_SIZE + 1;


  for (int i = start_row; i < end_row; ++i) {
    for (int j = start_col; j < end_col; ++j) {
      float r = 0.0f;
      for (int k = 0; k < MATRIX_SIZE; ++k) {
        const float e1 = m1.elements[i][k];
        const float e2 = m2.elements[k][j];
        r += e1 * e2;
      }
      result.elements[i][j] = r;
    }
  }
  
  int a = 2;
}

void b() {
  std::thread threads[THREADS_NUMBER];

  // Initialize threads
  Matrix m1, m2;
  m1.initialize_test();
  m2.initialize_test();

  Matrix r;
  for (int i = 0; i < THREADS_NUMBER; ++i) {
    threads[i] = std::thread(multiply_threading, std::ref(r), i, m1, m2);
  }

  r.print();

  for (int i = 0; i < THREADS_NUMBER; ++i) {
    threads[i].join();
  }

}

int main() {
  b();
  _sleep(10000);
}