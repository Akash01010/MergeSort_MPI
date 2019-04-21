# MergeSort_MPI
Parallelising Merge Sort using MPI programming.

To parallelize merge sort i have divided the data into multiple processor,sorted the subarrays at their respective processors and then combined the sorted results of all the processors. I have taken 1 million 5 digit numbers to sort.

Note: In my program the input size need not be multiple of no. of parallel processes program is executing. For achieving this, i have used MPI_Scatterv instead of MPI_Scatter.

To Run the program:
Step 1: Type mpicc -o mergeSort mergeSort.c -lm
Step 2: mpiexec -n <no_of_processes> ./mergeSort (it will automatically take input from a file named data.txt and sort it)
It will display the time taken to sort.(It will only print the time taken while sorting and not includes time for input and output taken by program)


If you want to see the sorted array, then uncomment the last section of the code.
