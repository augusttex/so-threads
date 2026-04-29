/* ===============================================================
   PROGRAMA: Comparação de Desempenho - Processamento Sequencial vs Paralelo
   OBJETIVO: Demonstrar o ganho de performance usando múltiplas threads
   =============================================================== */

#include <stdio.h>           // Entrada e saída (printf)
#include <windows.h>         // API do Windows para threads e funções de timing

/* ============ DEFINIÇÕES DE CONSTANTES ============ */
#define SIZE 100000000       // Tamanho do vetor: 100 MILHÕES de elementos
#define NUM_THREADS 4        // Número de threads paralelas que serão criadas

/* ============ VARIÁVEIS GLOBAIS ============ */
int vetor[SIZE];                    // Array gigante que será preenchido com dados
long long soma_parcial[NUM_THREADS]; // Armazena a soma parcial calculada por cada thread

/* ============ FUNÇÃO: MEDIR TEMPO ============
   Retorna o tempo atual do sistema em segundos (com alta precisão)
   Usada para cronometrar o tempo de execução dos cálculos
*/
double get_time() {
    LARGE_INTEGER frequency, counter;       // Estruturas para contadores de performance
    QueryPerformanceFrequency(&frequency);  // Obtém frequência do contador de performance
    QueryPerformanceCounter(&counter);      // Obtém valor atual do contador
    return (double)counter.QuadPart / (double)frequency.QuadPart;  // Converte para segundos
}

/* ============ FUNÇÃO: SOMAR VETOR (TAREFA DE CADA THREAD) ============
   Esta função é executada por cada thread independentemente.
   Cada thread calcula a soma de uma "fatia" do vetor, não do vetor inteiro.
   
   Argumentos:
   - lpParam: ponteiro para o ID da thread (qual thread está executando)
*/
DWORD WINAPI somar_vetor(LPVOID lpParam) {
    // Converte o argumento genérico para inteiro (ID da thread)
    int id = *((int*)lpParam);
    
    // PASSO 1: Calcular qual "fatia" do vetor esta thread vai processar
    int tamanho_fatia = SIZE / NUM_THREADS;  // Divide o vetor em 4 partes iguais
    int inicio = id * tamanho_fatia;         // Posição inicial desta thread
    // Última thread processa até o final (evita perder elementos por arredondamento)
    int fim = (id == NUM_THREADS - 1) ? SIZE : inicio + tamanho_fatia;

    // PASSO 2: Calcular a soma apenas da fatia desta thread
    long long soma_local = 0;
    for (int i = inicio; i < fim; i++) {
        // Realiza operação aritmética com cada elemento (multiplica por 5, divide por 5)
        soma_local += (vetor[i] * 5) / 5;
    }

    // PASSO 3: Armazenar resultado no array global (índice = ID da thread)
    soma_parcial[id] = soma_local;

    // Retorna 0 (sucesso na thread do Windows)
    return 0;
}

/* ============ FUNÇÃO PRINCIPAL ============ */
int main() {
    // ===== ETAPA 1: PREENCHIMENTO DO VETOR =====
    printf("Preenchendo vetor gigante (Aguarde um momento)...\n");
    for (int i = 0; i < SIZE; i++) {
        vetor[i] = 1;  // Preenche todos os 100 milhões de elementos com valor 1
    }

    // ===== ETAPA 2: CÁLCULO SEQUENCIAL (1 thread) =====
    printf("\nCalculando modo Sequencial...\n");
    double start_seq = get_time();  // Marca tempo inicial

    // Uma única thread faz a soma de todo o vetor
    long long soma_seq = 0;
    for (int i = 0; i < SIZE; i++) {
        soma_seq += (vetor[i] * 5) / 5;  // Operação: (1 * 5) / 5 = 1
    }

    double tempo_seq = get_time() - start_seq;  // Calcula tempo gasto
    printf("Soma: %I64d | Tempo: %f segundos\n", soma_seq, tempo_seq);

    // ===== ETAPA 3: CÁLCULO PARALELO (4 threads) =====
    printf("\nCalculando modo Paralelo (4 Threads)...\n");
    double start_par = get_time();  // Marca tempo inicial

    // Cria as estruturas para armazenar as threads e seus IDs
    HANDLE threads[NUM_THREADS];     // Handles (identificadores) das threads do Windows
    int ids_threads[NUM_THREADS];    // ID de cada thread (0, 1, 2, 3)

    // PASSO A: Criar 4 threads, cada uma com um ID diferente
    for (int i = 0; i < NUM_THREADS; i++) {
        ids_threads[i] = i;
        // CreateThread cria uma nova thread que executa a função somar_vetor
        threads[i] = CreateThread(
            NULL,                   // Atributos de segurança padrão
            0,                      // Tamanho de stack padrão
            somar_vetor,            // Função a ser executada
            &ids_threads[i],        // Argumento (ID da thread)
            0,                      // Flags (executar imediatamente)
            NULL                    // Pointer para ID da thread (não usado aqui)
        );

        if (threads[i] == NULL) {
            printf("Erro ao criar thread %d\n", i);
            return 1;
        }
    }

    // PASSO B: Aguardar que TODAS as threads terminem
    // WaitForMultipleObjects espera por TODOS os handles (threads)
    WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);

    // Somar todos os resultados parciais
    long long soma_total_paralela = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        soma_total_paralela += soma_parcial[i];
        // Fechar o handle da thread (liberar recursos)
        CloseHandle(threads[i]);
    }

    double tempo_par = get_time() - start_par;  // Calcula tempo gasto
    printf("Soma: %I64d | Tempo: %f segundos\n", soma_total_paralela, tempo_par);

    // ===== ETAPA 4: COMPARAÇÃO DOS RESULTADOS =====
    double speedup = tempo_seq / tempo_par;  // Calcula quantas vezes mais rápido foi
    printf("\n=== RESULTADO ===\n");
    printf("O codigo paralelo foi %.2fx mais rapido!\n", speedup);

    return 0;
}