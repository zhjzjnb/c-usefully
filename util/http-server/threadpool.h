

#ifndef HTTP_SERVER_THREADPOOL_H
#define HTTP_SERVER_THREADPOOL_H

#include <pthread.h>


typedef struct{
    void* (* function)(void*);
    void* arg;
} thread_task;


/*线程池管理*/
typedef struct{
    pthread_mutex_t lock;
    pthread_t* works;
    int worklen;
    int shutdown;

    thread_task* task_queue;
    int head;
    int tail;
    int cap;
    int qlen;

    pthread_cond_t q_full;     // 任务队列是满了
    pthread_cond_t q_empty;    // 任务队列是不是空了
} threadpool_t;

void err_exit(const char* err);

void threadpool_init(threadpool_t* pool, int works, int qlen);
void threadpool_submit(threadpool_t* pool,thread_task task);
void threadpool_destroy(threadpool_t *pool);

#endif //HTTP_SERVER_THREADPOOL_H
