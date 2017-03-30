#include <iostream>
#include <random>
#include <time.h>

static const int MATRIX_SIZE = 3;

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




int main() {
  // Time library
  Matrix m1, m2;
  m1.initialize_test();
  m2.initialize_test();


  Matrix r = m1.multiply(m2);
  r.print();

  _sleep(10000);
}