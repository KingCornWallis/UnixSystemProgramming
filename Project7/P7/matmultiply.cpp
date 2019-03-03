
// CPP Program to multiply two matrix using pthreads 
#include <bits/stdc++.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <ctime>

#define MAX_THREAD 4

using namespace std; 


struct holder
{
    int N;
    float* MATA; 
    float* MATB; 
    float* MATC;
    int flag = -1;
    int step_i = 0;
};
  
void* multiply(void* arg);
void funcA(float *MATA, float *MATB, float *MATC, int N);
void funcB(float *MATA, float *MATB, float *MATC, int N);
int position(int a, int b, int c);
void print(float *MATA, float *MATB, float *MATC, int N);


pthread_mutex_t mutex_total;
pthread_cond_t mutex_max;

int threadSig = 0;

// Driver Code 
int main(int argc, char **argv) 
{

    //Protect Global data with mutex
    if(argc != 3)
    {
        cout << "Please provide arguments and try again" << endl;
        exit(1);
    }

    if(!(strcmp(argv[1],"s") == 0 || strcmp(argv[1], "m") == 0))
    {
        cout << "Invalid 1st argument" << endl;
        exit(2);
    }    

    string size = argv[2];

    if(!isdigit(size[0]))
    {
        cout << "Invalid 2nd argument" << endl;
        exit(3);
    }
    int N = atoi(argv[2]);

    float* MATA = new float[N * N]; 
    float* MATB = new float[N * N]; 
    float* MATC = new float[N * N];

    srand(time(NULL));

    // Generating random values in matA and matB 
    for (int i = 0; i < N*N; i++) 
    { 
        MATA[i] = rand() % 10; 
        MATB[i] = rand() % 10; 
    } 

    pthread_mutex_init (&mutex_total, NULL);
    pthread_cond_init (&mutex_max, NULL);


    if(strcmp(argv[1], "s") == 0)
    {
        funcA(MATA, MATB, MATC, N);
    }
    else if(strcmp(argv[1], "m") == 0)
    {
        funcB(MATA, MATB, MATC, N);
    }
    else
    {
        cout << "Error: something broke" << endl;
        exit(4);
    }       
  
    print(MATA, MATB, MATC, N);

    delete [] MATA;
    delete [] MATB;  
    return 0; 
} 

void funcA(float *MATA, float *MATB, float *MATC, int N)
{
    threadSig = 1;
    pthread_attr_t attr;
    pthread_t thread;

    struct holder args;
    args.MATA = MATA;
    args.MATB = MATB;
    args.MATC = MATC;
    args.N = N; 
    
    //create a thread only to use multiply function here
    pthread_create (&thread, NULL, &multiply, &args);

    while(threadSig != 0)
    {
        //NO-OP
    }
}


void funcB(float *MATA, float *MATB, float *MATC, int N)
{
    pthread_t threads[MAX_THREAD];
    threadSig = 0;

    for (int i = 0; i < MAX_THREAD ; i++) 
    {
        struct holder args;
        args.MATA = MATA;
        args.MATB = MATB;
        args.MATC = MATC;
        args.N = N;
        //args.step_i = curr;
	    //args.flag = min(size, curr+num_line);


        //if(curr>=size) break;
        threadSig++;
        pthread_create (&threads[i], NULL, &multiply, &args);
    }

    // joining and waiting for all threads to complete 
    for (int i = 0; i < MAX_THREAD; i++)
    {
        pthread_join(threads[i], NULL);
    }
    while(threadSig != 0)
    {
        //NO-OP
    }
}

void* multiply(void* arg) 
{ 
    //Make an instance of our object and initialize components from our data structure
    struct holder *args = (holder *)arg;
    int sizeN = args->N;
    float* matA = args->MATA;
    float* matB = args->MATB;
    float* matC = args->MATC;
    
    int step = args->step_i;
    int flg = args->flag;
    
    float count = 0;

    if (flg == -1) 
    {
        flg = sizeN;
    }
     
    for (int i = 0; i < sizeN; i++) 
    {
        for (int j = 0; j < sizeN; j++) 
        {
            
            for (int k = 0; k < sizeN; k++) 
            {
                count += matA[position(sizeN, i, k)] * matB[position(sizeN, k, j)];
            }
            matC[position(sizeN, i, j)] = count;
        }
    }
    threadSig--;
    pthread_exit (NULL);
} 
  
int position(int a, int b, int c) 
{ 
    return b * a + c; 
}

void print(float *MATA, float *MATB, float *MATC, int N)
{
    float *arr[3] = {MATA, MATB, MATC};
    for(int index = 0; index < sizeof(arr); index++)
    { 
        for(int i = 0; i < N; i++)
        {
	    for(int j = 0; j < N; j++)
	    {
	        cout << arr[index][position(N, i, j)] << " ";
	    }
	    cout << endl;
        }
    }
}