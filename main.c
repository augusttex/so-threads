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

int main() {
    printf("Preenchendo vetor gigante (Aguarde um momento)...\n");
    for (int i = 0; i < SIZE; i++) {
        vetor[i] = 1;
    }

    printf("\nCalculando modo Sequencial...\n");
    double start_seq = get_time();

    long long soma_seq = 0;
    for (int i = 0; i < SIZE; i++) {
        soma_seq += (vetor[i] * 5) / 5;
    }

    double tempo_seq = get_time() - start_seq;
    printf("Soma: %lld | Tempo: %f segundos\n", soma_seq, tempo_seq);

    printf("\nCalculando modo Paralelo (4 Threads)...\n");
    double start_par = get_time();

    pthread_t threads[NUM_THREADS];
    int ids_threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        ids_threads[i] = i;
        pthread_create(&threads[i], NULL, somar_vetor, &ids_threads[i]);
    }

    long long soma_total_paralela = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        soma_total_paralela += soma_parcial[i];
    }

    double tempo_par = get_time() - start_par;
    printf("Soma: %lld | Tempo: %f segundos\n", soma_total_paralela, tempo_par);

    double speedup = tempo_seq / tempo_par;
    printf("\n=== RESULTADO ===\n");
    printf("O codigo paralelo foi %.2fx mais rapido!\n", speedup);

    return 0;
}