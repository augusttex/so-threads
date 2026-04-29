#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 100000000
#define NUM_THREADS 4

int vetor[SIZE];
long long soma_parcial[NUM_THREADS];

