# Project2

Our project was designed like a normal C++ project. It is not a standalone module that can be imported by other files.

The program does not take any command line arguments, however it does ask for user inputs. Once it begins, it asks for the 
dimensions of both matrices. It makes creates the 2 matrices and fills them in with the value of 1. Currently, it does not
have a functionality where it can let the user specify the values of the matrices. This was done so because we the programmers
can use worse case vaolues as the defaulted values to test the performance. 

After creating the matrices, including the results matrix, the program asks the user for which of the 3 tests they want to
test. 3 separate functions were made for each test. For the cache access test, we chose to use the Blocking algorithm. This is
where the multiplications are performed in blocks of the matrices, reducing cache misses. 

To compile the program, the command that must be used when g++ is being utilized is, 
```g++ -Wall -Wextra -g matrix.cpp```

Due to platform issues however, our device would fail compilation with the error, ```undefined reference to 'pthread_create'```.
To fix this, the compilation command was changed to 
```g++ -pthread -Wall -Wextra -g matrix.cpp```. 

In order to run the program, the command that is used is ```./a.out```.
