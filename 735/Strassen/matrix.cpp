#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <string>
#include <time.h>
#include <cstring>
#include <omp.h>

int N;
int k_prime;
typedef struct {
int width;
int height; 
float* elements;
} Matrix;
void printMatrix(Matrix A);
bool Correctness(Matrix* A,Matrix* B)
{
    for(int i=0;i<A->width;i++)
    {
        for(int j=0;j<A->width;j++)
        {
            if(A->elements[i*A->width + j] != B->elements[i*A->width+j])
            {
                return false;
            }
        }
    }
    return true;
}
Matrix* MatrixMultiply(Matrix* A,Matrix* B,Matrix* C)
{   
    
    for(int i=0;i<A->width;i++)
    {
        for(int j=0;j<A->width;j++)
        {
            for(int k=0;k<A->width;k++)
            {
                C->elements[i*A->width + j] += A->elements[i*A->width + k]*B->elements[k*A->width + j];
            }
        }
    }
    return C;
}
Matrix* MatrixAdd(Matrix* A,Matrix* B,bool sub=false)
{
    Matrix* C = new Matrix;
    C->width = A->width;C->height=A->height;C->elements = new float[C->width*C->height];
    for(int i=0;i<A->width;i++)
    {
        for(int j=0;j<A->width;j++)
        {
            if(sub)
            {
                C->elements[i*A->width+j] = A->elements[i*A->width+j] - B->elements[i*B->width+j];
            }
            else{
                C->elements[i*A->width+j] = A->elements[i*A->width+j] + B->elements[i*B->width+j];
            }
        }
    }
    return C;
}
Matrix * pMatrixMultiply(Matrix* A,Matrix *B,Matrix* C)
{
#pragma omp parallel for collapse(2)
    for(int i=0;i<A->width;i++)
    {
        for(int j=0;j<A->width;j++)
        {
            for(int k=0;k<A->width;k++)
            {
                C->elements[i*A->width + j] += A->elements[i*A->width + k]*B->elements[k*A->width + j];
            }
        }
    }

    return C;
}
int multiplier(int i,int size,int thread_num)
{
    int mul = 0;
    //printf("case: %d\n",thread_num%4);
    switch (thread_num % 4)
    {
    case 0:
        mul = i*size*2;
        break;
    case 1:
        mul = size + i*size*2;
        break;
    case 2:
        mul = size*2*size + i*size*2;
        break;
    case 3:
        mul = size*2*size + size +i*size*2;
        break;
    }
    return mul;
}
Matrix* seqStrassen(Matrix* A,Matrix* B,int n)
{
    int size = n;
    if(size == k_prime)
    {
        Matrix* C = new Matrix;
        C->width = size;
        C->height = size;
        C->elements = new float[size*size];
        C = pMatrixMultiply(A,B,C);
        return C;
    }
    else
    {
        //split into smaller matrices and recurse
        size = size/2;
        Matrix* A11 = new Matrix;Matrix* A12 = new Matrix;Matrix* A21 = new Matrix;Matrix* A22 = new Matrix;
        Matrix* B11 = new Matrix;Matrix* B12 = new Matrix;Matrix* B21 = new Matrix;Matrix* B22 = new Matrix;

        A11->width = size;A11->height = size;A12->width = size;A12->height = size;A21->width = size;A21->height = size;A22->width = size;A22->height = size;
        B11->width = size;B11->height = size;B12->width = size;B12->height = size;B21->width = size;B21->height = size;B22->width = size;B22->height = size;
        A11->elements = new float[size*size];A12->elements = new float[size*size];A21->elements = new float[size*size];A22->elements = new float[size*size];
        B11->elements = new float[size*size];B12->elements = new float[size*size];B21->elements = new float[size*size];B22->elements = new float[size*size];
        for(int i=0;i<size;i++)
        {
            memcpy(A11->elements+(i*size),A->elements+(i*size*2),size*sizeof(float));
            memcpy(A12->elements+(i*size),A->elements+(size + i*size*2),size*sizeof(float));
            memcpy(A21->elements+(i*size),A->elements+(size*2*size + i*size*2),size*sizeof(float));
            memcpy(A22->elements+(i*size),A->elements+(size*2*size + size +i*size*2),size*sizeof(float));

            memcpy(B11->elements+(i*size),B->elements+(i*size*2),size*sizeof(float));
            memcpy(B12->elements+(i*size),B->elements+(size + i*size*2),size*sizeof(float));
            memcpy(B21->elements+(i*size),B->elements+(size*2*size + i*size*2),size*sizeof(float));
            memcpy(B22->elements+(i*size),B->elements+(size*2*size + size +i*size*2),size*sizeof(float));
        }
        Matrix* M1 = seqStrassen(MatrixAdd(A11,A22),MatrixAdd(B11,B22),size);
        Matrix* M2 = seqStrassen(MatrixAdd(A21,A22),B11,size);
        Matrix* M3 = seqStrassen(A11,MatrixAdd(B12,B22,true),size);
        Matrix* M4 = seqStrassen(A22,MatrixAdd(B21,B11,true),size);
        Matrix* M5 = seqStrassen(MatrixAdd(A11,A12),B22,size);
        Matrix* M6 = seqStrassen(MatrixAdd(A21,A11,true),MatrixAdd(B11,B12),size);
        Matrix* M7 = seqStrassen(MatrixAdd(A12,A22,true),MatrixAdd(B21,B22),size);

        Matrix* C = new Matrix;
        //printf("Size: %d\n",n);

        C->width = size*2;C->height = size*2;C->elements = new float[size*2*size*2];
        Matrix* C11 = MatrixAdd(MatrixAdd(MatrixAdd(M1,M4),M5,true),M7);
        Matrix* C12 = MatrixAdd(M3,M5);
        Matrix* C21 = MatrixAdd(M2,M4);
        Matrix* C22 = MatrixAdd(MatrixAdd(MatrixAdd(M1,M2,true),M3),M6);

        int s2 = 0;
        for(int i=0;i<size;i++)
        {
            memcpy(C->elements+s2,C11->elements+(size*i),size*sizeof(float));s2+=size;
            memcpy(C->elements+s2,C12->elements+(size*i),size*sizeof(float));s2+=size;       
        }
        for(int i=0;i<size;i++)
        {
            memcpy(C->elements+s2,C21->elements+(size*i),size*sizeof(float));s2+=size;
            memcpy(C->elements+s2,C22->elements+(size*i),size*sizeof(float));s2+=size;
        }
        return C;
        
    }
}
Matrix* Strassen(Matrix* A,Matrix* B,int n)
{
    //printf("Size: %d\n",n);
    //printf("Thread Num: %d\n",omp_get_thread_num());
    int size = n;
    if(size == k_prime)
    {
        Matrix* C = new Matrix;
        C->width = size;
        C->height = size;
        C->elements = new float[size*size];
        C = pMatrixMultiply(A,B,C);
        //printMatrix(C);
        return C;
    }
    else
    {
        //split into smaller matrices and recurse
        size = size/2;
        //printf("%d\n",size);
        Matrix* A11 = new Matrix;Matrix* A12 = new Matrix;Matrix* A21 = new Matrix;Matrix* A22 = new Matrix;
        Matrix* B11 = new Matrix;Matrix* B12 = new Matrix;Matrix* B21 = new Matrix;Matrix* B22 = new Matrix;
        //Matrix* subs[8] = {&A11,&A12,&A21,&A22,&B11,&B12,&B21,&B22};
        //int* multipliers = new int[4];
        /*omp_set_dynamic(0);
        //omp_set_num_threads(8);
        #pragma omp parallel default(none) shared(A,B,size,subs) num_threads(8)
        {
            Matrix* A_p = subs[omp_get_thread_num()];
            A_p->width = size;A_p->height = size;A_p->elements = new float[size*size];

            int thread = omp_get_thread_num();
            //printf("thread: %d\n",thread);
            //omp_set_num_threads(2);
            #pragma omp parallel default(none) shared(A_p,A,B,thread,size) num_threads(size)
            {
                if(thread < 4)
                    {
                        memcpy(A_p->elements+(omp_get_thread_num()*size),A.elements+(multiplier(omp_get_thread_num(),size,thread)),size*sizeof(float));
                    }
                else{
                        memcpy(A_p->elements+(omp_get_thread_num()*size),B.elements+(multiplier(omp_get_thread_num(),size,thread)),size*sizeof(float));
                    }
            }
                
        }
        */
        //#pragma omp barrier  

        A11->width = size;A11->height = size;A12->width = size;A12->height = size;A21->width = size;A21->height = size;A22->width = size;A22->height = size;
        B11->width = size;B11->height = size;B12->width = size;B12->height = size;B21->width = size;B21->height = size;B22->width = size;B22->height = size;
        A11->elements = new float[size*size];A12->elements = new float[size*size];A21->elements = new float[size*size];A22->elements = new float[size*size];
        B11->elements = new float[size*size];B12->elements = new float[size*size];B21->elements = new float[size*size];B22->elements = new float[size*size];
        for(int i=0;i<size;i++)
        {
            memcpy(A11->elements+(i*size),A->elements+(i*size*2),size*sizeof(float));
            memcpy(A12->elements+(i*size),A->elements+(size + i*size*2),size*sizeof(float));
            memcpy(A21->elements+(i*size),A->elements+(size*2*size + i*size*2),size*sizeof(float));
            memcpy(A22->elements+(i*size),A->elements+(size*2*size + size +i*size*2),size*sizeof(float));

            memcpy(B11->elements+(i*size),B->elements+(i*size*2),size*sizeof(float));
            memcpy(B12->elements+(i*size),B->elements+(size + i*size*2),size*sizeof(float));
            memcpy(B21->elements+(i*size),B->elements+(size*2*size + i*size*2),size*sizeof(float));
            memcpy(B22->elements+(i*size),B->elements+(size*2*size + size +i*size*2),size*sizeof(float));
        }
        
        //Matrix M1,M2,M3,M4,M5,M6,M7;
        /*Matrix* Ms[7] = {&M1,&M2,&M3,&M4,&M5,&M6,&M7};

        #pragma omp parallel default(none) shared(Ms,A11,A12,A21,A22,B11,B12,B21,B22,size,M1,M2,M3,M4,M5,M6,M7) num_threads(7)
        {
            //printf("%d %d\n",omp_get_thread_num(),size);
            //printf("**%d**\n",omp_get_thread_num());
            switch(omp_get_thread_num())
            {
                
                case 0:
                    M1.width = size;M1.height = size;M1.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(MatrixAdd(A11,A22),MatrixAdd(B11,B22),size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(MatrixAdd(A11,A22),MatrixAdd(B11,B22),size));
                    break;
                
                case 1:
                    M2.width = size;M2.height = size;M2.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(MatrixAdd(A21,A22),B11,size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(MatrixAdd(A21,A22),B11,size));
                    break;
                case 2:
                    M3.width = size;M3.height = size;M3.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(A11,MatrixAdd(B12,B22,true),size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(A11,MatrixAdd(B12,B22,true),size));
                    break;
                case 3:
                    M4.width = size;M4.height = size;M4.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(A22,MatrixAdd(B21,B11,true),size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(A22,MatrixAdd(B21,B11,true),size));
                    break;
                case 4:
                    M5.width = size;M5.height = size;M5.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(MatrixAdd(A11,A12),B22,size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(MatrixAdd(A11,A12),B22,size));
                    break;
                case 5:
                    M6.width = size;M6.height = size;M6.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(MatrixAdd(A21,A11,true),MatrixAdd(B11,B12),size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(MatrixAdd(A21,A11,true),MatrixAdd(B11,B12),size));
                    break;
                case 6:
                    M7.width = size;M7.height = size;M7.elements = new float[size*size];
                    memcpy(Ms[omp_get_thread_num()]->elements,Strassen(MatrixAdd(A12,A22,true),MatrixAdd(B21,B22),size).elements,size*size*sizeof(float));
                    //Ms[omp_get_thread_num()] = &(Strassen(MatrixAdd(A12,A22,true),MatrixAdd(B21,B22),size));
                    break;
            }
            //#pragma omp barrier
        }
        */
        Matrix* M1;Matrix* M2;Matrix* M3;Matrix* M4;Matrix* M5;Matrix* M6;Matrix* M7;
        //#pragma omp single
        //{
            #pragma omp task shared(M1,A11,A22,B11,B22)
            {
                M1 = Strassen(MatrixAdd(A11,A22),MatrixAdd(B11,B22),size);
            }
            #pragma omp task shared(M2,A21,A22,B11)
            {
            M2 = Strassen(MatrixAdd(A21,A22),B11,size);
            }
            #pragma omp task shared(M3,A11,B12,B22)
            {
                M3 = Strassen(A11,MatrixAdd(B12,B22,true),size);
            }
            #pragma omp task shared(M4,B21,B11)
            {
                M4 = Strassen(A22,MatrixAdd(B21,B11,true),size);
            }
            #pragma omp task shared(M5,A11,A12,B22)
            {
                M5 = Strassen(MatrixAdd(A11,A12),B22,size);
            }
            #pragma omp task shared(M6,A21,A11,B11,B12)
            {
                M6 = Strassen(MatrixAdd(A21,A11,true),MatrixAdd(B11,B12),size);
            }
            #pragma omp task shared(M7,A12,A22,B21,B22)
            {
                M7 = Strassen(MatrixAdd(A12,A22,true),MatrixAdd(B21,B22),size);
            }
            #pragma omp taskwait
        //}
        //printf("corrects: %d\n",Correctness(M1,M1_));
        //#pragma omp barrier
        Matrix* C = new Matrix;
        //printf("Size: %d\n",n);

        C->width = size*2;C->height = size*2;C->elements = new float[size*2*size*2];
        Matrix* C11 = MatrixAdd(MatrixAdd(MatrixAdd(M1,M4),M5,true),M7);
        Matrix* C12 = MatrixAdd(M3,M5);
        Matrix* C21 = MatrixAdd(M2,M4);
        Matrix* C22 = MatrixAdd(MatrixAdd(MatrixAdd(M1,M2,true),M3),M6);

        int s2 = 0;
        for(int i=0;i<size;i++)
        {
            memcpy(C->elements+s2,C11->elements+(size*i),size*sizeof(float));s2+=size;
            memcpy(C->elements+s2,C12->elements+(size*i),size*sizeof(float));s2+=size;       
        }
        for(int i=0;i<size;i++)
        {
            memcpy(C->elements+s2,C21->elements+(size*i),size*sizeof(float));s2+=size;
            memcpy(C->elements+s2,C22->elements+(size*i),size*sizeof(float));s2+=size;
        }
        //#pragma omp barrier
        return C;
        
        //return A;
    }
}
void printMatrix(Matrix A)
{
    for (int i=0;i<A.height;i++)
    {
        for(int j=0;j<A.width;j++)
        {
            printf("%.0f ",A.elements[i*A.width + j]);
        }
        printf("\b\n");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    struct timespec start,stop;
    int num_threads = 8;
    if(argc < 2)
    {
        printf("Usage: ./matrix [k] [k'] (optional) [n] (optional). Where matrices are size 2^k and k' is the terminal matrix size.\n2^n is the number of threads. Default k' is 2. Default value of n is 3\n");
    }
    
    int k = std::atoi(argv[1]);
    if(argc > 2)
    {
        k_prime = std::atoi(argv[2]);
    }
    else{
        k_prime = 2;
    }
    if (argc > 3) {num_threads = pow(2,atoi(argv[3]));}
    //omp_set_max_active_levels(k);
    omp_set_num_threads(num_threads);

    N = pow(2,k);
    //printf("%d\n",N);
    //populate random matrices
    srand (time(NULL));

    Matrix* A = new Matrix;
    Matrix* B = new Matrix;
    Matrix* C = new Matrix;
    A->width = N;A->height=N;A->elements = new float[N*N];
    B->width = N;B->height=N;B->elements = new float[N*N];
    C->width = N;C->height = N;C->elements = new float[N*N];

    for(int i=0;i<N*N;i++)
    {
        A->elements[i] = rand() % 10;
        B->elements[i] = rand() % 10;
        C->elements[i] = 0;
    }
    //omp_set_num_threads(num_threads);
    //printMatrix(A);
    //printMatrix(B);
    clock_gettime(CLOCK_REALTIME, &start);
    Matrix* C_normal = MatrixMultiply(A,B,C);
    clock_gettime(CLOCK_REALTIME, &stop);
    double total_time_bf=(stop.tv_sec-start.tv_sec)
	+0.000000001*(stop.tv_nsec-start.tv_nsec);
    //printMatrix(C_normal);
    clock_gettime(CLOCK_REALTIME, &start);

    Matrix* Res;
    #pragma omp parallel 
    {
        #pragma omp single
        {
            Res = Strassen(A,B,N);    
        }
        clock_gettime(CLOCK_REALTIME, &stop);

    }

    clock_gettime(CLOCK_REALTIME, &stop);
    double total_time = (stop.tv_sec-start.tv_sec)
	+0.000000001*(stop.tv_nsec-start.tv_nsec);
    

    //printMatrix(Res);
    printf("Correct: %d, k=%d k'=%d n=%d brute force time= %0.2f strassen time = %0.2f\n",Correctness(C_normal,Res),k,k_prime,num_threads,total_time_bf,total_time);

}   
