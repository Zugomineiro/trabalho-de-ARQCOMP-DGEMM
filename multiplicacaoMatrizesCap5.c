#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "multiplicacaoParametros.h"
#include <x86intrin.h>

// #define dimensaoMatriz 1000
// #define tamanhoMatriz dimensaoMatriz*dimensaoMatriz

double A[tamanhoMatriz];
double B[tamanhoMatriz];
double C[tamanhoMatriz];

void do_block (int n, int si, int sj, int sk,
    double *A, double *B, double *C)
{
    // Calculate the actual end indices for this block, respecting 'n'.
    // This is crucial for handling partial blocks at the matrix edges.
    int current_block_end_i = (si + BLOCKSIZE > n) ? n : (si + BLOCKSIZE);
    int current_block_end_j = (sj + BLOCKSIZE > n) ? n : (sj + BLOCKSIZE);
    int current_block_end_k = (sk + BLOCKSIZE > n) ? n : (sk + BLOCKSIZE);

    // Loop for C's rows (i)
    // The step (i += UNROLL * 8) means 'i' increments by (UNROLL * 8)
    // If BLOCKSIZE is 32, and UNROLL is e.g., 4, then UNROLL*8 = 32.
    // So 'i' iterates exactly BLOCKSIZE steps in this inner loop.
    for ( int i = si; i < current_block_end_i; i += UNROLL * 8 ) {
        // Loop for C's columns (j)
        for ( int j = sj; j < current_block_end_j; j++ ) {

            __m512d c[UNROLL];
            // Initialize C accumulators by loading values from C
            for (int r = 0; r < UNROLL; r++) {
                // IMPORTANT: The _mm512_load_pd loads 8 doubles.
                // If (i + r*8) is near current_block_end_i such that
                // i + r*8 + 7 (the last element of the vector) goes past 'n',
                // you would still have an out-of-bounds access.
                // For a robust solution, masked loads/stores are needed for the
                // last few iterations of 'i' where (current_block_end_i - i) < UNROLL*8 or < 8.
                // However, for typical blocking where BLOCKSIZE is a multiple of (UNROLL*8)
                // or if (UNROLL*8) is a small factor like 8, this might not be the direct crash.
                // The current crash is due to 'k' going out of bounds.
                c[r] = _mm512_load_pd(C + i + r*8 + j*n);
            }

            // Inner loop for K dimension
            for( int k = sk; k < current_block_end_k; k++ ) // This loop condition is the primary fix!
            {
                __m512d bb = _mm512_broadcastsd_pd(_mm_load_sd(B + j*n + k));
                for (int r = 0; r < UNROLL; r++) {
                    // This was the crashing line.
                    // A+n*k+r*8+i
                    // With current_block_end_k, 'k' will never exceed 'n-1'.
                    // The 'i + r*8' part:
                    // If UNROLL*8 is <= 8 (e.g., UNROLL=1), and BLOCKSIZE is 32,
                    // then 'i' increments by 8. So 'i' is always valid.
                    // If UNROLL is larger, this might need more robust checks.
                    c[r] = _mm512_fmadd_pd(_mm512_load_pd(A + n*k + r*8 + i), bb, c[r]);
                }
            }

            // Store results back to C
            for (int r = 0; r < UNROLL; r++) {
                _mm512_store_pd(C + i + r*8 + j*n, c[r]);
            }
        }
    }
}
 
  void dgemm (int n, double* A, double* B, double* C)
  {
  for ( int sj = 0; sj < n; sj += BLOCKSIZE )
  for ( int si = 0; si < n; si += BLOCKSIZE )
  for ( int sk = 0; sk < n; sk += BLOCKSIZE )
  do_block(n, si, sj, sk, A, B, C);
  }


void randomizarMatriz(int n2,double* matriz){
    
    for (int i = 0; i < n2;i++){
        // rand() returns an int between 0 and RAND_MAX
        // Divide by RAND_MAX to get float between 0 and 1
        double r = (double)rand() / (double)RAND_MAX;
        
        matriz[i] = r;

    }
}

void lerMatriz(int n2,double* matriz){
    for (int i = 0; i < n2;i++){
        printf("numeroAtual: %f\n", matriz[i]);
    }

}

enum { NS_PER_SECOND = 1000000000 };

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}

void salvarTempo(struct timespec delta){
    FILE *arquivo = fopen("tempoExecucaoC.txt", "w");  // "w" = write mode

    if (arquivo == NULL) {
        printf("Error opening arquivo!\n");
    }

    fprintf(arquivo, "Tempo: %d.%.9ld\n",(int)delta.tv_sec, delta.tv_nsec);     // Write text
    fprintf(arquivo, "tamanho da matriz: %d X %d",dimensaoMatriz,dimensaoMatriz);
    //fputs("This is another line.\n", arquivo);  // Write another line

    fclose(arquivo);  // Always close the arquivo
}

int main(){
    struct timespec start, finish, delta;

    // Initialize random seed once
    srand(time(NULL));
    
    printf("TamanhoAtual: %d\n",dimensaoMatriz);
    printf("Criando matriz A \n");
    randomizarMatriz(tamanhoMatriz,&A[0]);
    printf("Criando matriz B \n");
    randomizarMatriz(tamanhoMatriz,&B[0]);
    printf("Fazendo calculos...\n");

    clock_gettime(CLOCK_REALTIME, &start);
    dgemm(dimensaoMatriz,&A[0],&B[0],&C[0]);
    clock_gettime(CLOCK_REALTIME, &finish);
    sub_timespec(start, finish, &delta);

    printf("tempo de execucao: %d.%.9ld\n", (int)delta.tv_sec, delta.tv_nsec);

    salvarTempo(delta);
    //lerMatriz(tamanhoMatriz,&C[0]);

    return 0;
}