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
    int square_size = N * N;
    for (int i = 0; i < H; i++) {
        if ( (i + 1) % nthreads == id )
        {
            for (int j = 0; j < W; j++) {
                float tmp_front = 0.0, tmp_rest = 0.0;
                // result记录N <= 8的累积和向量, result_rest记录 N > 8的累积和向量
                vec_t result_8 = vec_set1_float(0.0);
                vec_t result_rest =  vec_set1_float(0.0);
                for (int ii = 0; ii < N; ii++)
                {
                    vec_t front_8 = vec_load(input + (i + ii) * (W + N - 1) + j);
                    result_8 = vec_add(result_8, front_8);
                    if (N >= 8)
                    {
                        vec_t rest = vec_load(input + (i + ii) * (W + N - 1) + j + 8);
                        result_rest = vec_add(rest, result_rest);
                    }
                }
                for (int k = 0; k < N; ++k)
                    tmp_front += result_8[k];
                for (int k = 8; k < N; ++k)
                    tmp_rest += result_rest[k];
                // tmp保存最终存放的结果
                float tmp = (tmp_front + tmp_rest) / square_size;
                output[i * W + j] = tmp;
            }
        }
    }

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

    for (int i = 0; i < H; i++) {
        if ( (i + 1) % nthreads == id)
        {
            for (int j = 0; j < W; j++) {
                float tmp = 0.0;
                for (int ii = 0; ii < N; ii++)
                    for (int jj = 0; jj < N; jj++)
                    {
                        tmp += input[(i + ii) * (W + N - 1) + j + jj];
                    }
                tmp = tmp / (N * N);
                output[i * W + j] = tmp;
            }
        }
    }

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
        int ret = pthread_create(&tids[i], NULL, solve_part2, &params);
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
