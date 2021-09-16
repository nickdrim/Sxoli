#ifndef JOB_SCHEDULER
#define JOB_SCHEDULER

#include <pthread.h>
#include <sys/types.h>

typedef void (*work_function)(batch_pointer test);

typedef struct job{
  work_function func;
  batch_pointer arg;
}job,*job_ptr;

typedef struct queue_list{
  job_ptr work;
  struct queue_list* next;
  struct queue_list* prev;
}queue_list,*QNode;

typedef struct queue{
  QNode first;
  QNode last;
  int no_jobs;
}queue,*queue_ptr;

typedef struct job_scheduler{
  int execution_threads;
  int no_work;
  int working;
  int alive;
  pthread_mutex_t mutex;
  pthread_t *tids;
  pthread_cond_t cond;
  pthread_cond_t end_cond;
  queue_ptr q;
  int stop;
}job_scheduler,*scheduler;


queue_ptr queue_initialize(void);
void queue_push(queue_ptr q, job_ptr new_work);
QNode queue_pop(queue_ptr q);
int queue_empty(queue_ptr q);
void free_queue(queue_ptr q);

QNode create_queue_node(job_ptr new_work);
QNode insert_queue_node(QNode tail, job_ptr new_work);
void free_queue_node(QNode head);

job_ptr create_new_job(work_function function, batch_pointer arguments);
void *thread_pool(scheduler fifo);
scheduler create_scheduler(int no_threads,void* function,int no);
void submit_job(scheduler fifo, job_ptr new_work);
QNode execute_job(scheduler fifo);
void destroy_job(QNode work_done);
void wait_all_tasks_finish(scheduler fifo);
void reschedule(scheduler fifo,void* functon,int no);
void destroy_scheduler(scheduler fifo);


#endif