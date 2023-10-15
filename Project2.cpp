#include <iostream>
#include <pthread.h>
#include <thread>
#include <list>
#include <chrono>
#include <vector>

using namespace std;

int column = 0;

void multiply(int beginning_row, int end_row, vector<vector<int> >&matrix1, vector<vector<int> > &matrix2,
  vector<vector<int> > &results){

  for(int i = beginning_row; i < end_row; i++){
        for(int j = 0; j < int(matrix2[0].size()); j++){
            for(int k = 0; k < int(matrix1[0].size()); k++){
                results[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void multi_thread(vector<vector<int> >&matrix1, vector<vector<int> > &matrix2, vector<vector<int> > &results){
  vector<thread> threads;
  int num_threads;
  cout << "Enter number of threads: ";
  cin >> num_threads;

  int thread_rows = matrix1.size()/num_threads;            // Calculate the number of rows needed
  int current_row = 0;

  for(int i = 0; i < num_threads; i++){
    threads.push_back(thread(multiply, current_row, current_row+thread_rows, ref(matrix1), ref(matrix2), ref(results)));
    current_row+=thread_rows;
  }

  for(int i = 0; i < num_threads; i++){         // Force all the threads to finish before moving on
    threads[i].join();
  }
}

void display_matrix(vector<vector<int> > &results){
  for(int i = 0; i < int(results.size()); i++){
    for(int j = 0; j < int(results[0].size()); j++){
      cout << results[i][j] << ' ';
    }
    cout << endl;
  }
}

void SIMD(){

}

void blocking(vector<vector<int> >& matrix1, vector<vector<int> >& matrix2, vector<vector<int> >& results, int blockSize){
  int row1 = matrix1.size();
  int col1 = matrix1[0].size();
  int col2 = matrix2[0].size();

  for(int block1 = 0; block1 < row1; block1 += blockSize){
    for(int block2 = 0; block2 < col2; block2 += blockSize){
      for(int block3 = 0; block3 < col1; block3 += blockSize){
        for(int i = block1; i < std::min(block1 + blockSize, row1); i++){
          for(int j = block2; j < std::min(block2 + blockSize, col2); j++){
            for(int k = block3; k < std::min(block3 + blockSize, col1); k++){
              results[i][j] += matrix1[i][k] * matrix2[k][j];
            }
          }
        }
      }
    }
  }
}

void make_matrix(int row, int col, vector<vector<int> >&vec, int val){
  for(int i = 0; i < row; i++){
    vector<int> vect(col, val);
    vec.push_back(vect);
  }
}

int main(){
    int row1, row2, col1, col2;

    // Input
    cout << "Enter the rows and columns for matrix 1: ";
    cin >> row1 >> col1;
    cout << "Enter the rows and columns for matrix 2: ";
    cin >> row2 >> col2;
    column = col2;

    if (col1 != row2) {
        cout << "Invalid matrix sizes." << endl;
        return 1;
    }

    // Make the matrix
    std::vector<std::vector<int> > matrix1, matrix2, results;
    make_matrix(row1, col1, matrix1, 1);
    make_matrix(row2, col2, matrix2, 1);
    make_matrix(row1, col2, results, 0);

    // Option
    int option;
    cout << "Enter 1 for threads, 2 for SIMD, and 3 for blocking: ";
    cin >> option;

    if(option == 1){
      multi_thread(matrix1, matrix2, results);
    }else if(option == 2){
      SIMD();
    }else{
      cout << "Enter the blockSize: ";
      cin >> option;
      blocking(matrix1, matrix2, results, option);
    }

    display_matrix(results);
    return 0;
}
