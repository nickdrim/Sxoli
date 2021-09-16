#include "header.h"

int queue_empty(queue_ptr q){
  if(q == NULL) return 1;
  else if(q->first == NULL) return 1;

  return 0;
}

queue_ptr queue_initialize(void){
  queue_ptr temp = malloc(sizeof(queue));
  temp->first = NULL;
  temp->last = NULL;
  temp->no_jobs = 0;

  return temp;
}

void queue_push(queue_ptr q, job_ptr new_work){
  if(queue_empty(q)){
    q->first = create_queue_node(new_work);
    q->last = q->first;
    q->no_jobs++;
  }else{
    q->last = insert_queue_node(q->last, new_work);
    q->no_jobs++;
  }
}

QNode queue_pop(queue_ptr q){
  if(queue_empty(q)) return NULL;
  
  QNode temp = NULL;

  if(q->first == q->last){
    temp = q->first;
    q->first = NULL;
    q->last = NULL;
    return temp;
  }else{
    temp = q->last;
    q->last = q->last->prev;
  }

    q->no_jobs--;
    temp->next = NULL;
    temp->prev = NULL;

    return temp;
}

void free_queue(queue_ptr q){
  if(q == NULL) return;
  if(queue_empty(q)) free(q);
  else{
    q->last = NULL;
    free_queue_node(q->first);
    free(q);
  }
}

void free_queue_node(QNode head){
  if(head == NULL) return;
  free_queue_node(head->next);
  free(head);
}

QNode create_queue_node(job_ptr new_work){
  QNode temp = malloc(sizeof(queue_list));

  temp->next = NULL;
  temp->prev = NULL;
  temp->work = new_work;

  return temp;
}

QNode insert_queue_node(QNode tail, job_ptr new_work){
  QNode temp = create_queue_node(new_work);

  tail->next = temp;
  temp->prev = tail;

  return temp;
}

job_ptr create_new_job(work_function function, batch_pointer arguments){
  job_ptr temp = malloc(sizeof(job));
  temp->func = function;
  temp->arg = arguments;

  return temp;
}

scheduler create_scheduler(int no_threads, void* function, int no){
  scheduler temp = malloc(sizeof(job_scheduler));

  temp->execution_threads = no_threads;
  temp->no_work = no;
  temp->stop = 0;
  temp->alive = no_threads;
  temp->working = 0;
  temp->q = queue_initialize();

  if(pthread_mutex_init(&(temp->mutex),NULL)){
    perror("mutex init :");
    exit(1);
  }

  pthread_cond_init(&(temp->cond), NULL);
  pthread_cond_init(&(temp->end_cond), NULL);

  temp->tids = malloc(sizeof(pthread_t) * no_threads);
  for(int i = 0 ; i < no_threads ; i++){
    temp->tids[i] = pthread_create(&(temp->tids[i]), NULL, function, temp);
  }

  return temp;
}

void reschedule(scheduler fifo,void* function,int no){
  if(fifo == NULL || function == NULL) return;

  fifo->no_work = no;
  fifo->stop = 0;
  fifo->alive = fifo->execution_threads;
  fifo->working = 0;

  pthread_mutex_destroy(&(fifo->mutex));
  pthread_cond_destroy(&(fifo->cond));
  pthread_cond_destroy(&(fifo->end_cond));

  if(pthread_mutex_init(&(fifo->mutex),NULL)){
    perror("mutex init :");
    exit(1);
  }

  pthread_cond_init(&(fifo->cond), NULL);
  pthread_cond_init(&(fifo->end_cond), NULL);

  for(int i = 0 ; i < fifo->execution_threads ; i++){
    fifo->tids[i] = pthread_create(&(fifo->tids[i]), NULL, function, fifo);
  }
}

void destroy_scheduler(scheduler fifo){
  if(fifo == NULL) return;

  pthread_mutex_destroy(&(fifo->mutex));
  pthread_cond_destroy(&(fifo->cond));
  pthread_cond_destroy(&(fifo->end_cond));

  free(fifo->tids);

  free_queue(fifo->q);
  free(fifo);
}

void submit_job(scheduler fifo, job_ptr new_work){
  pthread_mutex_lock(&(fifo->mutex));
  queue_push(fifo->q,new_work);
  //printf("job submited\n");
  pthread_cond_broadcast(&(fifo->cond));
  pthread_mutex_unlock(&(fifo->mutex));
}

void wait_all_tasks_finish(scheduler fifo){
  if(fifo == NULL) return;
  pthread_mutex_lock(&(fifo->mutex));
  while(1){
    if((!fifo->stop && fifo->working) || (fifo->alive != 0 && fifo->stop)){
      pthread_cond_wait(&(fifo->end_cond), &(fifo->mutex));
    }else{
      break;  
    }
  }
  pthread_mutex_unlock(&(fifo->mutex));

  for(int i = 0 ; i < fifo->execution_threads ; i++){
    pthread_join(fifo->tids[i], NULL);
  }

}

QNode execute_job(scheduler fifo){
  return queue_pop(fifo->q);
}

void destroy_job(QNode work_done){
  free(work_done->work);
  free(work_done);
}