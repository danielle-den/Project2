#include <iostream>
#include <pthread.h>
#include <thread>
#include <string>
#include <list>
#include <random>
#include <chrono>
#include <vector>
#include <intrin.h>
#include <immintrin.h>
#include <smmintrin.h>

using namespace std;

template <typename M>
class matrix {
public:
    matrix();

    void initialize();
    void SIMD();
    static void multiply(int beginning_row, int end_row, vector<vector<M> >& matrix1, vector<vector<M> >& matrix2,
        vector<vector<M> >& results);

    void multi_thread();
    void display_matrix();
    void make_matrix(int row, int col, vector<vector<M> >& vec, int val);
    void blocking(int blockSize);

    void output();

private:
    int row1, row2, col1, col2, column;
    bool data_type, display_output;
    int duration;
    vector<vector<M> > results, matrix1, matrix2;
};

// Constructor
template <typename M>
matrix<M>::matrix() {
    this->row1 = this->row2 = this->col1 = this->col2 = this->column = 0;
    this->initialize();
}

template <typename M>
void matrix<M>::initialize() {
    // Get rows and columns
    cout << "Enter the rows and columns for matrix 1: ";
    cin >> row1 >> col1;
    cout << "Enter the rows and columns for matrix 2: ";
    cin >> row2 >> col2;
    column = col2;

    if (col1 != row2) {
        cout << "Invalid matrix sizes." << endl;
        std::exit(1);
    }

    // Fill up matrix with fixed-point/floating point values
    cout << "Enter 1 for floating point operation, 0 for fixed-point: ";
    cin >> this->data_type;

    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister 19937 generator
    double random_number;

    if (this->data_type) {
        // Create a uniform real distribution
        std::uniform_real_distribution<double> dis(0, 100000);
        random_number = dis(gen);
    }
    else {
        std::uniform_int_distribution<int> dis(0 * 1000, 100000 * 1000);
        int random_int = dis(gen);
        random_number = static_cast<double>(random_int) / 1000;
    }

    make_matrix(row1, col1, matrix1, random_number);
    make_matrix(row2, col2, matrix2, random_number);
    make_matrix(row1, col2, results, 0);


    // Option
    int option;
    cout << "Enter 1 for threads, 2 for SIMD, and 3 for blocking: ";
    cin >> option;

    if (option == 1) {
        multi_thread();
    }
    else if (option == 2) {
        SIMD();
    }
    else {
        cout << "Enter the blockSize: ";
        cin >> option;
        blocking(option);
    }

    cout << "Operation time: " << this->duration << " (ms). \n" << endl;
}

// int column = 0;

// Multiply matrices
template <typename M>
void matrix<M>::multiply(int beginning_row, int end_row, vector<vector<M> >& matrix1, vector<vector<M> >& matrix2,
    vector<vector<M> >& results) {
    for (int i = beginning_row; i < end_row; i++) {
        for (int j = 0; j < int(matrix2[0].size()); j++) {
            for (int k = 0; k < int(matrix1[0].size()); k++) {
                results[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

template <typename M>
void matrix<M>::multi_thread() {
    vector<thread> threads;
    int num_threads;
    cout << "Enter number of threads between 1 and " << std::thread::hardware_concurrency() << " :";
    cin >> num_threads;

    int thread_rows = matrix1.size() / num_threads;            // Calculate the number of rows needed
    int current_row = 0;

    for (int i = 0; i < num_threads; i++) {
        threads.push_back(thread(multiply, current_row, current_row + thread_rows, ref(matrix1), ref(matrix2), ref(results)));
        current_row += thread_rows;
    }

    for (int i = 0; i < num_threads; i++) {         // Force all the threads to finish before moving on
        threads[i].join();
    }
}

template <typename M>
void matrix<M>::display_matrix() {
    for (int i = 0; i < int(results.size()); i++) {
        for (int j = 0; j < int(results[0].size()); j++) {
            cout << results[i][j] << ' ';
        }
        cout << endl;
    }
}

template <typename M>
void matrix<M>::SIMD() {
    int size = matrix2.size();
    M arr1 = new M[size];
    M arr2 = new M[size];

    for (int i = 0; i < size; i++) {
        __m256i mask = _mm256_setr_epi32(-5, -3, -2, -6, -3, -5, -2, -9);


        //get the values for the result matrix
        for (int k = 0; k < size; k++) {

            //load row & column matrixies
            for (int j = 0; j < size; j++) {
                arr1[j] = matrix1[i][j];
                arr2[j] = matrix2[j][k];
            }

            //create correct data type objects
            __m256i newArr1 = _mm256_maskload_epi32(arr1, mask);
            __m256i newArr2 = _mm256_maskload_epi32(arr2, mask);

            //multiply correct row & colum values together
            __m256i newMult = _mm256_mul_epi32(newArr1, newArr2);

            //add values together & set final matrix
            int* res1 = (int*)&newMult;
            results[i][k] = int(res1[0]) + int(res1[1]) + int(res1[3]) + \
                int(res1[4]) + int(res1[5]) + int(res1[6]) + int(res1[7]);
        }
    }

    delete[] arr1;
    delete[] arr2;
}

// Blocking algorithm
template <typename M>
void matrix<M>::blocking(int blockSize) {
    int row1 = matrix1.size();
    int col1 = this->matrix1[0].size();
    int col2 = this->matrix2[0].size();

    // Break up the matrices into different blocks and compute the multiplication
    auto start = std::chrono::high_resolution_clock::now();
    for (int block1 = 0; block1 < row1; block1 += blockSize) {
        for (int block2 = 0; block2 < col2; block2 += blockSize) {
            for (int block3 = 0; block3 < col1; block3 += blockSize) {
                for (int i = block1; i < std::min(block1 + blockSize, row1); i++) {
                    for (int j = block2; j < std::min(block2 + blockSize, col2); j++) {
                        for (int k = block3; k < std::min(block3 + blockSize, col1); k++) {
                            results[i][j] += matrix1[i][k] * matrix2[k][j];
                        }
                    }
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

}

// Create a matrix of this size
template <typename M>
void matrix<M>::make_matrix(int row, int col, vector<vector<M> >& vec, int val) {
    for (int i = 0; i < row; i++) {
        vector<M> vect(col, val);
        vec.push_back(vect);
    }
}


int main() {
    matrix<float> new_matrix;

    return 0;
}
