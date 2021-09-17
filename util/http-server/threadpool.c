
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>

void* work_func(void* arg) {
    threadpool_t* pool = arg;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        while (pool->qlen == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->q_empty, &pool->lock);
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }

        pool->qlen--;
        thread_task* task = &pool->task_queue[pool->head];

        pool->head++;
        if (pool->head == pool->cap) {
            pool->head = 0;
        }

        pthread_cond_signal(&pool->q_full);
        pthread_mutex_unlock(&pool->lock);

        task->function(task->arg);
    }
    return NULL;
}


void err_exit(const char* err) {
    perror(err);
    exit(1);
}

void threadpool_init(threadpool_t* pool, int works, int qlen) {
    pthread_mutex_init(&pool->lock, NULL);

    pthread_cond_init(&pool->q_full, NULL);
    pthread_cond_init(&pool->q_empty, NULL);

    pool->shutdown = 0;
    pool->head = 0;
    pool->cap = qlen;
    pool->tail = 0;
    pool->qlen = 0;
    pool->task_queue = malloc(sizeof(thread_task) * qlen);
    pool->worklen = works;
    pool->works = malloc(sizeof(pthread_t) * works);
    for (int i = 0; i < works; ++i) {
        if (pthread_create(&pool->works[i], NULL, work_func, pool)) {
            err_exit("create work error");
        }
    }
}

void threadpool_submit(threadpool_t* pool, thread_task task) {
//    printf("submit a task\n");
    pthread_mutex_lock(&pool->lock);


    while (pool->qlen == pool->cap && !pool->shutdown) {

        pthread_cond_wait(&pool->q_full, &pool->lock);
    }
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->lock);
        return;
    }
    pool->qlen++;
    pool->task_queue[pool->tail++] = task;

    if (pool->tail == pool->cap) {
        pool->tail = 0;
    }

    pthread_cond_signal(&pool->q_empty);

    pthread_mutex_unlock(&pool->lock);
}

void threadpool_destroy(threadpool_t* pool) {

    pool->shutdown = 1;

    pthread_cond_broadcast(&pool->q_empty);

    for (int i = 0; i < pool->worklen; ++i) {
        pthread_join(pool->works[i], NULL);
        printf("work%d exit\n", i);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->q_full);
    pthread_cond_destroy(&pool->q_empty);
    free(pool->works);
    free(pool->task_queue);
}