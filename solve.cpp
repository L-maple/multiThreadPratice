#include "simd.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <immintrin.h>

#define NUM_THREADS 8

using namespace std;

typedef struct {
    float* input;
    float* output;
    int W;
    int H;
    int N;
    int nthreads;
}Param;

int id = -1;
pthread_mutex_t mutex;
int get_id()
{
    pthread_mutex_lock(&mutex);
    id = id + 1;
    pthread_mutex_unlock(&mutex);
    return id;
}

void print(int W, int H, float *input)
{
    for (int row = 0; row < H; ++row)
    {
        for (int col = 0; col < W; ++col)
        {
            int row_count = row * W;
            int col_count = col;
            printf("%f ", *(input + row_count + col_count));
        }
        printf("\n");
    }
    printf("\n");
}


// 使用多线程和SIMD指令进行优化
void* solve_part1(void* params)
{
    Param* ps = (Param*)params;
    int W = ps->W;
    int H = ps->H;
    int N = ps->N;
    int nthreads = ps->nthreads;
    float* input = ps->input;
    float* output = ps->output;

    int id = get_id();
    // printf("id: %d, %d %d %d\n", id, W, H, N);

    for (int i = 0; i < H; i++) {
        // printf("%d %d %d\n", i+1, nthreads, id);
        if ( (i + 1) % nthreads == id)
        {
            //printf("i: %d\n", i);
            for (int j = 0; j < W; j++) {
                float tmp = 0.0;
                vec_t result = vec_set1_float(0.0);
                for (int ii = 0; ii < N; ii++)
                {
                    vec_t front_8 = vec_load(input + (i + ii) * (W + N - 1) + j);
                    result = vec_add(result, front_8);
                }
                for (int i = 0; i < N; ++i)
                    tmp += result[i];
                tmp = tmp / (N * N);
                output[i * W + j] = tmp;
                // printf("output_index: %d\n", i * W + j);
            }
        }
    }
    // print(W, H, output);

    return 0;
}


// 单纯用多线程进行优化
void* solve_part2(void* params)
{
    Param* ps = (Param*)params;
    int W = ps->W;
    int H = ps->H;
    int N = ps->N;
    int nthreads = ps->nthreads;
    float* input = ps->input;
    float* output = ps->output;

    int id = get_id();
    // printf("id: %d, %d %d %d\n", id, W, H, N);

    for (int i = 0; i < H; i++) {
        // printf("%d %d %d\n", i+1, nthreads, id);
        if ( (i + 1) % nthreads == id)
        {
            //printf("i: %d\n", i);
            for (int j = 0; j < W; j++) {
                float tmp = 0.0;
                for (int ii = 0; ii < N; ii++)
                    for (int jj = 0; jj < N; jj++)
                    {
                        //printf("input_index: %d\n", (i + ii) * (W + N - 1) + j + jj);
                        tmp += input[(i + ii) * (W + N - 1) + j + jj];
                    }
                tmp = tmp / (N * N);
                output[i * W + j] = tmp;
                // printf("output_index: %d\n", i * W + j);
            }
        }
    }
    // print(W, H, output);

    return 0;
}


/*思路:
(1) 将任务分给NUM_THREADS个线程；
*/
void solve(int W, int H, int N, float *input, float *output)
{
    pthread_mutex_init(&mutex, NULL);
    // 定义线程的 id 变量，多个变量使用数组
    int nthreads = H < NUM_THREADS ? H : NUM_THREADS;
    pthread_t* tids = (pthread_t*)malloc(sizeof(pthread_t) * nthreads);
    if (tids == NULL) exit(1);

    Param params = {input, output, W, H, N, nthreads};
    for(int i = 0; i < nthreads; ++i)
    {
        // 参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
        int ret = pthread_create(&tids[i], NULL, solve_part1, &params);
        if (ret != 0)
        {
            fprintf(stdout, "pthread_create error: error_code=%d\n", ret);
        }
    }

    for (int i = 0; i < nthreads; ++i)
    {
        // 等待所有线程运行结束
        pthread_join(tids[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(tids);
}
