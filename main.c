#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 100000000
#define NUM_THREADS 4

int vetor[SIZE];
long long soma_parcial[NUM_THREADS];

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void* somar_vetor(void* arg) {
    int id = *((int*)arg);
    
    int tamanho_fatia = SIZE / NUM_THREADS;
    int inicio = id * tamanho_fatia;
    int fim = (id == NUM_THREADS - 1) ? SIZE : inicio + tamanho_fatia;

    long long soma_local = 0;

    for (int i = inicio; i < fim; i++) {
        soma_local += (vetor[i] * 5) / 5;
    }

    soma_parcial[id] = soma_local;

    return NULL;
}

