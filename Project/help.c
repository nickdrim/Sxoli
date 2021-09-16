#include "header.h"

weight_ptr weights = NULL;
scheduler global_scheduler = NULL;

//Add negative relationship to both cliques
void negative_relationship(pbucket head, pbucket data){
  if(head->arr == NULL){
    head->arr = malloc(sizeof(pbucket));
    head->arr_size++;
    head->arr[0] = data;
    
    if(data->arr == NULL){
      data->arr = malloc(sizeof(pbucket));
      data->arr_size++;
      data->arr[0] = head;
    }else{
      data->arr_size++;
      data->arr = realloc(data->arr, sizeof(pbucket) * data->arr_size);
      data->arr[data->arr_size - 1] = head;
    }

  }else{
    int flag = 1;
    for(int i = 0 ; i < head->arr_size ; i++){
      if(head->arr[i] == data){
        flag = 0;
        break;
      }
    }

    if(flag){
      head->arr_size++;
      head->arr = realloc(head->arr, sizeof(pbucket) * head->arr_size);
      head->arr[head->arr_size - 1] = data;

      if(data->arr == NULL){
        data->arr = malloc(sizeof(pbucket));
        data->arr_size++;
        data->arr[0] = head;
      }else{
        data->arr_size++;
        data->arr = realloc(data->arr, sizeof(pbucket) * data->arr_size);
        data->arr[data->arr_size - 1] = head;
      }
    }
  }
  
}

void create_negative_relationships(HTable records, full_set_ptr w_set){
  if(records == NULL || w_set == NULL) return;
  int total = 0;

  for(int i = 0 ; i < w_set->training_size ; i++){
    if(w_set->training_set[i]->expected == 1) continue;

    CNode temp1 = hashSearch(records, w_set->training_set[i]->string1);
    CNode temp2 = hashSearch(records, w_set->training_set[i]->string2);

    negative_relationship(temp1->clique, temp2->clique);
    temp1->clique->negative_relationships++;
    temp2->clique->negative_relationships++;
    total++;

  }
  for(int i = 0 ; i < w_set->test_size ; i++){
    if(w_set->test_set[i]->expected == 1) continue;

    CNode temp1 = hashSearch(records, w_set->test_set[i]->string1);
    CNode temp2 = hashSearch(records, w_set->test_set[i]->string2);

    negative_relationship(temp1->clique, temp2->clique);
    temp1->clique->negative_relationships++;
    temp2->clique->negative_relationships++;
    total++;
  }
  for(int i = 0 ; i < w_set->validation_size ; i++){
    if(w_set->validation_set[i]->expected == 1) continue;

    CNode temp1 = hashSearch(records, w_set->validation_set[i]->string1);
    CNode temp2 = hashSearch(records, w_set->validation_set[i]->string2);

    negative_relationship(temp1->clique, temp2->clique);
    temp1->clique->negative_relationships++;
    temp2->clique->negative_relationships++;
    total++;
  }

  printf("total = %d\n",total);
}

//Count negative relationships, print sum and return cliques
void count_and_print_negative(CNode list){
  if(list == NULL) return;
  
  CNode traverse = list;
  int clique_counter = 0;
  int individual_counter = 0;
  pbucket *visited = NULL;
  int currentSize = 0;
  int flag = 0;

  while(traverse != NULL){  
    flag = 1;
    if(visited == NULL){
      currentSize++;
      visited = malloc(sizeof(pbucket)*currentSize);
      visited[0] = traverse->clique;
    }else{
      for(int j=0 ; j < currentSize ; j++){
        if(visited[j] == traverse->clique){
          flag = 0;
          break;
        }
      }
      if(flag){
        currentSize++;
        visited = realloc(visited,sizeof(pbucket)*currentSize);
        visited[currentSize-1] = traverse->clique;
      }
    }
    if(flag){
      individual_counter += traverse->clique->negative_relationships;
      clique_counter += traverse->clique->arr_size;
    }
    
    traverse = traverse->right;
  }

  printf("Total clique negative relationships : %d \n",clique_counter);
  printf("Total individual negative relationships : %d \n",(int) individual_counter / 2);
  free(visited);

  return;
}

weight_ptr create_weights(void){
  weight_ptr temp = malloc(sizeof(weight));
  temp->w = malloc(sizeof(double) * 2 * vector_size());
  for(int i = 0 ; i < 2 * vector_size() ; i++){
      temp->w[i] = 0;
  }
  temp->b = 0;

  return temp;
}

void free_weights(weight_ptr temp){
  free(temp->w);
  free(temp);
  temp = NULL;
}

set_ptr create_set(char* item1, char* item2, int val){
  set_ptr temp = malloc(sizeof(set));
  
  if(item1 != NULL){
    temp->string1 = malloc(strlen(item1) + 1);
    strcpy(temp->string1,item1);
  }
  if(item2 != NULL){
    temp->string2 = malloc(strlen(item2) + 1);
    strcpy(temp->string2,item2);
  }
  temp->expected = val;

  return temp;

}

void free_full_set(full_set_ptr w_set){
  if(w_set == NULL) return;

  set_free(w_set->training_set, w_set->training_size);
  set_free(w_set->test_set, w_set->test_size);
  set_free(w_set->validation_set, w_set->validation_size);
  free(w_set);
}

void set_free(set_ptr* free_set, int size){
  if(free_set == NULL) return;

  for(int i = 0 ; i < size ; i++){
    if(free_set[i] == NULL) continue;
    free(free_set[i]->string1);
    free(free_set[i]->string2);
    free(free_set[i]);
  }

  free(free_set);
}

batch_pointer create_batch(int size_of_batch, ht_w* word_hash){
  batch_pointer temp = malloc(sizeof(batch));
  temp->batches = malloc(sizeof(vset_ptr) * size_of_batch);
  for(int i = 0 ; i < size_of_batch ; i++){
    temp->batches[i] = NULL;
  }
  temp->batch_size = size_of_batch;
  temp->words = word_hash;
  temp->successfull = 0;
  
  return temp;
}

vset_ptr create_vectorized_set(int current,set_ptr* cur_set, ht_w* word_hash){
  vset_ptr temp = malloc(sizeof(vectorized_set));
  temp->string1 = malloc(strlen(cur_set[current]->string1) + 1);
  strcpy(temp->string1, cur_set[current]->string1);
  temp->string2 = malloc(strlen(cur_set[current]->string2) + 1);
  strcpy(temp->string2, cur_set[current]->string2);
  temp->x_vector = vectorise(cur_set[current]->string1, cur_set[current]->string2, word_hash);
  temp->value = cur_set[current]->expected;

  return temp;
}

void batch_free(batch_pointer* w_set,int size){
  if(w_set == NULL) return;

  for(int i = 0 ; i < size ; i++){
    for(int j = 0 ; j < w_set[i]->batch_size ; j++){
      if(w_set[i]->batches[j]->string1 != NULL) free(w_set[i]->batches[j]->string1);
      if(w_set[i]->batches[j]->string2 != NULL) free(w_set[i]->batches[j]->string2);
      free(w_set[i]->batches[j]->x_vector);
      free(w_set[i]->batches[j]);
    }
    free(w_set[i]->batches);
    w_set[i]->words = NULL;
    free(w_set[i]);
  }
  free(w_set);
}

void resolve_transitivity(HTable records, full_set_ptr w_set, set_ptr* new_train, int size){
  if(w_set == NULL || new_train == NULL) return;
  printf("resolving transitivity issues between the existing training set and the new one\n");

  for(int i = 0 ; i < size ; i++){
    CNode temp1 = hashSearch(records, new_train[i]->string1);
    CNode temp2 = hashSearch(records, new_train[i]->string2);
    //positive relationship
    if(temp1 == temp2) continue;

    int flag = 0;
    //negative relationship
    for(int j = 0 ; j < temp1->clique->arr_size ; j++){
      if(temp1->clique->arr[j] == temp2->clique){
        flag = 1;
        break;
      }
    }

    if(flag) continue;

    printf("Added %s and %s with expected value = %d to training set\n",new_train[i]->string1, new_train[i]->string2, new_train[i]->expected);
    w_set->training_size++;
    w_set->training_set = realloc(w_set->training_set,sizeof(set_ptr) * w_set->training_size);
    w_set->training_set[w_set->training_size - 1] = create_set(new_train[i]->string1, new_train[i]->string2, new_train[i]->expected);
  }
}

full_set_ptr create_full_set(int size){
  full_set_ptr temp = malloc(sizeof(full_set));

  temp->test_size = size * 0.2;
  temp->validation_size = size * 0.2;
  temp->training_size = size - temp->test_size - temp->validation_size;

  temp->training_set = malloc(sizeof(set_ptr) * temp->training_size);
  for(int i = 0 ; i < temp->training_size ; i++){
    temp->training_set[i] = NULL;
  }

  temp->test_set = malloc(sizeof(set_ptr) * temp->test_size);
  for(int i = 0 ; i < temp->test_size ; i++){
    temp->test_set[i] = NULL;
  }

  temp->validation_set = malloc(sizeof(set_ptr) * temp->validation_size);
  for(int i = 0 ; i < temp->validation_size ; i++){
    temp->validation_set[i] = NULL;
  }

  return temp;
}



//train the model
void train_test_validate(full_set_ptr w_set, ht_w* word_hash){
  if(w_set == NULL || word_hash == NULL) return;

  weights = create_weights();

  long int number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

  int no_of_batches = w_set->training_size / BATCH_SIZE;
  int last_batch_size = w_set->training_size % BATCH_SIZE;
  int x;
  if(last_batch_size == 0){
    x = 0;
  }else{
    x = 1;
  }

  int counter = 0;
  int current = 0;

  scheduler fifo = create_scheduler(number_of_processors + 1, thread_pool, no_of_batches + x);
  global_scheduler = fifo;

  batch_pointer* w_batch = malloc(sizeof(batch_pointer) * (no_of_batches+ x));
  for(int i = 0 ; i < no_of_batches + x ; i++){
    if(counter == no_of_batches){
      w_batch[i] = create_batch(last_batch_size, word_hash);
      for(int j = 0 ; j < last_batch_size ; j++){
        w_batch[i]->batches[j] = create_vectorized_set(current, w_set->training_set, word_hash);
        current++;
      }
    }else{
      w_batch[i] = create_batch(BATCH_SIZE, word_hash);
      for(int j = 0 ; j < BATCH_SIZE ; j++){
        w_batch[i]->batches[j] = create_vectorized_set(current, w_set->training_set, word_hash);
        current++;
      }
    }
    counter++;
    submit_job(fifo, create_new_job(train, w_batch[i]));
  }

  wait_all_tasks_finish(fifo);

  batch_free(w_batch, no_of_batches + x);
  w_batch = NULL;

  no_of_batches = w_set->test_size / BATCH_SIZE;
  last_batch_size = w_set->test_size % BATCH_SIZE;
  if(last_batch_size == 0){
    x = 0;
  }else{
    x = 1;
  }

  counter = 0;
  current = 0;

  reschedule(fifo, thread_pool, no_of_batches + x);

  w_batch = malloc(sizeof(batch_pointer) * (no_of_batches+ x));
  for(int i = 0 ; i < no_of_batches + x ; i++){
    //printf("%d and %d\n", i,current);
    if(counter == no_of_batches){
      w_batch[i] = create_batch(last_batch_size, word_hash);
      for(int j = 0 ; j < last_batch_size ; j++){
        w_batch[i]->batches[j] = create_vectorized_set(current, w_set->test_set, word_hash);
        current++;
      }
    }else{
      w_batch[i] = create_batch(BATCH_SIZE, word_hash);
      for(int j = 0 ; j < BATCH_SIZE ; j++){
        w_batch[i]->batches[j] = create_vectorized_set(current, w_set->test_set, word_hash);
        current++;
      }
    }
    counter++;
    submit_job(fifo, create_new_job(test_set, w_batch[i]));
  }

  wait_all_tasks_finish(fifo);
  destroy_scheduler(fifo);

  int total = 0;
  int all = 0;
  for(int i = 0 ; i < no_of_batches + x ; i++){
    total += w_batch[i]->successfull;
    all += w_batch[i]->batch_size;
  }

  printf("\n\n\n\n\n\ntest accuracy = %f\n\n\n\n\n\n",(float) total/all);

  batch_free(w_batch, no_of_batches + x);
  w_batch = NULL;

  printf("\n\n\n\n\nVALIDATION\n\n\n\n\n");
  total = 0;
  for(int i = 0 ; i < w_set->validation_size ; i++){
    int* temp_vector = vectorise(w_set->validation_set[i]->string1,w_set->validation_set[i]->string2, word_hash);
    int estimate = validate(temp_vector);
    if(estimate == w_set->validation_set[i]->expected){
      total++;
    }
    free(temp_vector);
    printf("%s \t %s expected \t\t %d and got %d\n",w_set->validation_set[i]->string1,w_set->validation_set[i]->string2,w_set->validation_set[i]->expected,estimate);
  }

  printf("\n\n\n\nvalidation accuracy = %f\n\n\n\n",(float) total/w_set->validation_size);


  free_weights(weights);
}


//Logistic regression with gradient descent
void *thread_pool(scheduler fifo){

  while(1){
    pthread_mutex_lock(&(fifo->mutex));
    while(queue_empty(fifo->q) && !fifo->stop){
      pthread_cond_wait(&(fifo->cond), &(fifo)->mutex);
    }

    if(fifo->stop)
      break;
    

    QNode temp = execute_job(fifo);
    job_ptr do_work = NULL;
    if(temp!= NULL){
      do_work = temp->work;
      printf("thread %ld has started training\n",pthread_self());
      printf("threads alive = %d, no of work to be done = %d , stop condition %d\n",fifo->alive,fifo->no_work,fifo->stop);
      fifo->working++;
      if(fifo->no_work == 0 && queue_empty(fifo->q)){ 
        fifo->stop = 1;
        break;
      }
      pthread_mutex_unlock(&(fifo->mutex));
      
      do_work->func(do_work->arg);      
      
      destroy_job(temp);

      pthread_mutex_lock(&(fifo->mutex));
      fifo->working--;
      fifo->no_work--;
      if(fifo->no_work == 0){
        fifo->stop = 1;
        pthread_cond_broadcast(&(fifo->cond));
        break;
      }
      pthread_mutex_unlock(&(fifo->mutex));

    }else{
      pthread_mutex_unlock(&(fifo->mutex));
    }
  }

  fifo->alive--;
  pthread_cond_signal(&(fifo->end_cond));
  pthread_mutex_unlock(&(fifo->mutex));

  pthread_exit(NULL);
  return NULL;
}

void first_training(full_set_ptr w_set, ht_w* word_hash, HTable records){
  if(w_set == NULL || word_hash == NULL) return;

  weights = create_weights();
  set_ptr* training = w_set->training_set;
  int size = w_set->test_size;

  double threshold = 0.01;
  printf("first training\n");
  for(int i = 0 ; i < w_set->training_size ; i++){
    int* temp_vector = vectorise(training[i]->string1, training[i]->string2, word_hash);
    if(first_train(temp_vector, training[i]->expected)){
      free(temp_vector);
      break;
    }
    free(temp_vector);
  }
  printf("first training finished\n");
  
  set_ptr* new_training_set = NULL;
  int new_training_size = 0;

  printf("first testing\n");
  for(int x = 0 ; x < 1 ; x++){
    for(int i = 0 ; i < (int) size/100 ; i++){
      for(int y = 0 ; y < 1 ; y++){
        for(int j = i+1 ; j < (int) size/100 ; j++){
          char* temp_string1 = NULL;
          char* temp_string2 = NULL;

          if(x == 0){
            temp_string1 = w_set->test_set[i]->string1;
          }else{
            temp_string1 = w_set->test_set[i]->string2;
          }
          if(y == 0){
            temp_string2 = w_set->test_set[j]->string1;
          }else{
            temp_string2 = w_set->test_set[j]->string2;
          }

          if(!strcmp(temp_string1,temp_string2)) continue;

          int* temp_vector = vectorise(temp_string1, temp_string2, word_hash);
          double p = first_test(temp_vector);
          free(temp_vector);

          if( (p < threshold) || (p > 1 - threshold)){
            if(new_training_set == NULL){
              new_training_set = malloc(sizeof(set_ptr));
              new_training_set[0] = create_set(temp_string1,temp_string2,p);
              new_training_size++;
            }else{
              new_training_size++;
              new_training_set = realloc(new_training_set, sizeof(set_ptr) * new_training_size);
              new_training_set[new_training_size - 1] = create_set(temp_string1,temp_string2,p);
            }
          }

        }
      }
    }
  }
  printf("Testing finished\n");
  int temp = w_set->training_size;

  resolve_transitivity(records, w_set, new_training_set, new_training_size);
  printf("\n\nAfter resolving %d have been added to training test\n\n",w_set->training_size - temp);
  set_free(new_training_set, new_training_size);
  free_weights(weights);
  
}

int first_train(int *x_vector, int y){
  static int count_e = 0;
  int index_size = 0;
  int *indexes = sparse_array(x_vector, &index_size,NULL,0,0);
  double learning_rate;
  double e;
  if(y == 1){
    learning_rate = 0.005;
    e = 0.0001;
  }
  else{
    learning_rate = 0.00125;
    e = 0.001;
  }

  double inner_product = 0;
  for(int i = 0 ; i < index_size ; i++){
    inner_product += weights->w[indexes[i]] * x_vector[indexes[i]];
  }

  double t = inner_product + weights->b;

  double sigma = 1 / (1 + exp(-t));
  double error = sigma - y;

  double* new_w_vector = malloc(sizeof(double) * index_size);
  for(int i = 0 ; i < index_size ; i++){
    new_w_vector[i] = weights->w[indexes[i]] - (learning_rate)*error*x_vector[indexes[i]];
    //printf("w[%d] = %lf -> new[%d] = %lf\n",indexes[i],weights->w[indexes[i]],indexes[i],new_w_vector[i]);  
  }

  double sum = 0;

  for(int i = 0 ; i < index_size ; i++){
    sum += pow(fabs(new_w_vector[i] - weights->w[indexes[i]]),2.0);
  }

  

  if(sqrt(sum)< e){
    if(y == 1){
      for(int i = 0 ; i < index_size ; i++){
      weights->w[indexes[i]] = new_w_vector[i];
      }
      weights->b -= learning_rate *error;
      free(new_w_vector);
      free(indexes);
      return 0;
    }
    count_e++;
    if(count_e > 1000){
      count_e = 0;
      free(indexes);
      free(new_w_vector);
      return 1;
    }
    free(new_w_vector);
    free(indexes);
    return 0;
  }else{
    for(int i = 0 ; i < index_size ; i++){
      weights->w[indexes[i]] = new_w_vector[i];
    }
    weights->b -= learning_rate *error;
    free(new_w_vector);
    free(indexes);
    return 0;
  }
}

void train(batch_pointer test){
  double error = 0;
  double learning_rate = 0.01;
  int total_index_size = 0;
  int* total_indexes = initialize_index_vector();

  for(int i = 0 ; i < test->batch_size ; i++){
    int index_size = 0;
    int* indexes = sparse_array(test->batches[i]->x_vector, &index_size, total_indexes , &total_index_size,1);

    double inner_product = 0;

    for(int j = 0 ; j < index_size ; j++){
      inner_product += weights->w[indexes[j]] * test->batches[i]->x_vector[indexes[j]];
    }

    double t = inner_product + weights->b;

    double sigma = 1 / (1 + exp(-t));
    error += sigma - test->batches[i]->value;

    free(indexes);
  }

  pthread_mutex_lock(&(global_scheduler->mutex));
  for(int j = 0 ; j < total_index_size ; j++){
    weights->w[total_indexes[j]] -= (learning_rate * error) / test->batch_size;
  }

  weights->b -= (learning_rate * error) / test->batch_size;
  pthread_mutex_unlock(&(global_scheduler->mutex));

  free(total_indexes);
}

int* initialize_index_vector(void){
  int* temp = malloc(sizeof(int) * 2 * vector_size());

  for(int i = 0 ; i < 2 * vector_size() ; i++){
    temp[i] = -1;
  }

  return temp;
}

//return an array with the position of the non zero values of a vector
int* sparse_array(int* vector, int* index_size,int* total_indexes,int* total_index_size,int flag){
  if(vector == NULL) return NULL;

  int* indexes = NULL;
  int counter = 0;
  int counter2 = 0;

  for(int i = 0 ; i < vector_size() * 2 ; i++){
    if(vector[i] != 0){
      if(indexes == NULL){
        counter++;
        indexes = malloc(sizeof(int) * counter);
        indexes[counter - 1] = i;
      }else{
        counter++;
        indexes = realloc(indexes, sizeof(int) * counter);
        indexes[counter - 1] = i;
      }

      if(flag){
        if(total_indexes[i] == -1){
          counter2++;
          total_indexes[counter2 - 1] = i;
        }
      }
    }
  }

  *index_size = counter;
  if(flag) *total_index_size = counter2;

  return indexes;
}

void test_set(batch_pointer test){
  for(int i = 0 ; i < test->batch_size ; i++){
    int index_size = 0;
    int* indexes = sparse_array(test->batches[i]->x_vector, &index_size, NULL ,0,0);

    double inner_product = 0;
    for(int j = 0 ; j < index_size ; j++){
      inner_product += weights->w[indexes[j]] * test->batches[i]->x_vector[indexes[j]];
    }

    double t = inner_product + weights->b;

    double sigma = 1 / (1 + exp(-t));
    printf("%s \t %s\t\t expected %d and got %f\n",test->batches[i]->string1, test->batches[i]->string2, test->batches[i]->value, sigma);
    if(sigma > 0.5){
      if(test->batches[i]->value != 1){
        int* temp_vector = vectorise(test->batches[i]->string1, test->batches[i]->string2,test->words);
        first_train(temp_vector, test->batches[i]->value);
        free(temp_vector);
      }else{
        test->successfull++;
      }
    }else{
      if(test->batches[i]->value != 0){
        int* temp_vector = vectorise(test->batches[i]->string1, test->batches[i]->string2,test->words);
        first_train(temp_vector, test->batches[i]->value);
        free(temp_vector);
      }else{
        test->successfull++;
      }
    }

    free(indexes);
  }
}

int validate(int *x_vector){
  int index_size = 0;
  int* indexes = sparse_array(x_vector, &index_size, NULL ,0,0);

  double inner_product = 0;
  for(int i = 0 ; i < index_size ; i++){
    inner_product += weights->w[indexes[i]] * x_vector[indexes[i]];
  }

  double t = inner_product + weights->b;

  double sigma = 1 / (1 + exp(-t));
  free(indexes);

  if(sigma > 0.5){
    return 1;
  }else{
    return 0;
  }
}

//Sigma function
double first_test(int *x_vector){
  int index_size = 0;
  int *indexes = sparse_array(x_vector, &index_size,NULL,0,0);

  double inner_product = 0;
  for(int i = 0 ; i < index_size ; i++){
    inner_product += weights->w[indexes[i]] * x_vector[indexes[i]];
  }

  double t = inner_product + weights->b;

  double sigma = 1 / (1 + exp(-t));

  free(indexes);
  //printf("sigma = %lf\n",sigma);
  return sigma;
}

void wprint(full_set_ptr w_set){
  for(int i = 0 ; i < w_set->training_size ; i++){
    printf("%d\n",i);
    printf("strings %s - %s val = %d\n",w_set->training_set[i]->string1,w_set->training_set[i]->string2,w_set->training_set[i]->expected);
  }
  for(int i = 0 ; i < w_set->test_size ; i++){
    printf("strings %s - %s val = %d\n",w_set->test_set[i]->string1,w_set->test_set[i]->string2,w_set->test_set[i]->expected);
  }
  for(int i = 0 ; i < w_set->validation_size ; i++){
    printf("strings %s - %s val = %d\n",w_set->validation_set[i]->string1,w_set->validation_set[i]->string2,w_set->validation_set[i]->expected);
  }
}