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

void dgemm (int n, double* A, double* B, double* C){
 for (int i = 0; i < n; i += UNROLL*8)
    for (int j = 0; j < n; ++j)
    {
        __m512d cij[UNROLL];
        for (int r = 0; r < UNROLL; r++)
            cij[r] = _mm512_load_pd(C + i+ r*8 +j*n); /* cij = C[i][j] */

        for( int k = 0; k < n; k++ ){
            __m512d bb = _mm512_broadcastsd_pd(_mm_load_sd(B+j*n+k));

            for (int r=0;r<UNROLL; r++)
                cij[r] = _mm512_fmadd_pd(_mm512_load_pd(A + n*k + r*8 + i),bb,cij[r]);
        }

        for (int r = 0; r < UNROLL; r++)
            _mm512_store_pd(C+i+r*8+j*n, cij[r]); /* C[i][j] = c0 */
    }
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