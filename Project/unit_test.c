#include "header.h"
#include "acutest.h"
#define TABLE_SIZE 100000

void test_record_create(void){
  RNode list = newRecordNode("test","test");

  TEST_ASSERT(list != NULL);
  TEST_ASSERT(list->next == NULL);
  TEST_ASSERT(strcmp(list->name,"test")==0);
  TEST_ASSERT(strcmp(list->path,"test")==0);
  
  recordFreeList(list);
}

void test_record_insert(void){
  char* temp = malloc(strlen("test")+1);
  strcpy(temp,"test");
  RNode list = NULL;

  recordListInsert(&list,temp,temp);

  TEST_ASSERT(list != NULL);
  TEST_ASSERT(list->next == NULL);
  TEST_ASSERT(strcmp(list->name,"test")==0);
  TEST_ASSERT(strcmp(list->path,"test")==0);

  recordFreeList(list);
  free(temp);

}

void test_insert(void){
  RNode list = NULL;

  RNode temp = newRecordNode("test","test1");

  insert(&list,temp);

  TEST_ASSERT(list == temp);
  TEST_ASSERT(list->next == temp->next);
  TEST_ASSERT(strcmp(list->name,temp->name)==0);
  TEST_ASSERT(strcmp(list->path,temp->path)==0);

  free(list->name);
  free(list->path);
  free(list);
}

void test_clique_create(void){
  CNode list = newCliqueNode("test");

  TEST_ASSERT(list != NULL);
  TEST_ASSERT(list->clique->info == NULL);
  TEST_ASSERT(list->right == NULL);
  TEST_ASSERT(strcmp(list->key,"test") == 0);

  cliqueFreeList(list);
}

void test_clique_insert(void){
  int N = 20;
  char **arr = malloc(sizeof(char *)* N);

  CNode list = NULL;

  for(int i = 0 ; i < N ; i++){
    arr[i] = malloc(strlen("test")+1);
    strcpy(arr[i],"test");

    cliqueListInsert(&list,NULL,arr[i],arr[i]);

    TEST_ASSERT(strcmp(list->key,arr[i]) == 0);
    TEST_ASSERT(list->clique->info != NULL);
  }

  CNode node = list;

  for (int i = N - 1; i >= 0 ; i--) {
		TEST_ASSERT(strcmp(node->key,arr[i]) == 0);
    TEST_ASSERT(node->clique->info != NULL);
		node = node->right;
	}

  cliqueFreeList(list);
  for(int i = 0 ; i < N ; i++){
    free(arr[i]);
  }
  free(arr);
}

void test_clique_search(void){
  
  CNode list = NULL;
  int N = 20;
  char **arr = malloc(sizeof(char *)* N);
  char buffer[50];
  
  for(int i = 0 ; i < N ; i++){
    sprintf(buffer,"%d",i);
    arr[i] = malloc(strlen(buffer)+1);
    strcpy(arr[i],buffer);

    cliqueListInsert(&list,NULL,arr[i],arr[i]);
  }

  CNode node = list;

  for(int i = N-1 ; i >=0 ; i--){
    CNode found_node = cliqueListSearch(list, arr[i]);
    
    TEST_ASSERT(found_node == node);
    TEST_ASSERT(strcmp(found_node->key,arr[i]) == 0);

    node = node->right;
  }

  cliqueFreeList(list);
  for(int i = 0 ; i < N ; i++){
    free(arr[i]);
  }
  free(arr);
}

void test_hash_initialize(void){
  float loadFactor = 0.6;
  HTable records = hashInitialize(10);
  int size = 10 / loadFactor;
  
  TEST_ASSERT(records != NULL);

  for(int i=0 ; i < size ; i++){
    TEST_ASSERT(records[i].start == NULL);
  }

  hashFree(records);

}

void test_hash_insert(void){
  HTable records = hashInitialize(10);

  CNode list = NULL;
  int N = 20;
  char **arr = malloc(sizeof(char *)* N);
  char buffer[50];
  
  for(int i = 0 ; i < N ; i++){
    sprintf(buffer,"%d",i);
    arr[i] = malloc(strlen(buffer)+1);
    strcpy(arr[i],buffer);

    cliqueListInsert(&list,records,arr[i],arr[i]);

    CNode temp1 = cliqueListSearch(list,arr[i]);
    CNode temp2 = hashSearch(records,arr[i]);

    TEST_ASSERT(temp1 == temp2);

    int h = hash(arr[i]);
    int flag = 1;

    TEST_ASSERT(records[h].start != NULL);
    if(records[h].start->link != NULL){
      HNode traverse = records[h].start;
      while(traverse != NULL){
        if(strcmp(records[h].start->item,arr[i])){
          TEST_ASSERT(strcmp(records[h].start->item,arr[i]) == 0);
          flag = 0;
          break;
        }
        traverse = traverse->link;
      }
    }
    if(flag == 1) TEST_ASSERT(strcmp(records[h].start->item,arr[i]) == 0);
    TEST_ASSERT(records[h].start->top == temp1);
  }

  cliqueFreeList(list);
	hashFree(records);
  for(int i=0 ; i < N ; i++){
    free(arr[i]);
  }
  free(arr);

}

void test_hash_search(void){
  float loadFactor = 0.6;
  HTable records = hashInitialize(10);
  int size = 10 / loadFactor;

  CNode list = NULL;
  int N = 20;
  char **arr = malloc(sizeof(char *)* N);
  char buffer[50];
  
  for(int i = 0 ; i < N ; i++){
    sprintf(buffer,"%d",i);
    arr[i] = malloc(strlen(buffer)+1);
    strcpy(arr[i],buffer);
  
    list = newCliqueNode(arr[i]);
    hashInsert(records,list);

    TEST_ASSERT(list == hashSearch(records,list->key));
  }

  list = NULL;

  for(int i = 0 ; i < size ; i++){
    if(records[i].start!=NULL){
      HNode traverse = records[i].start;
      while(traverse != NULL){
        TEST_ASSERT(traverse->top == hashSearch(records,traverse->top->key));
        traverse = traverse->link;
      }
    }
  }

  for(int i = 0 ; i < size ; i++){
    if(records[i].start!=NULL){
      HNode traverse = records[i].start;
      while(traverse != NULL){
        cliqueFreeList(traverse->top);
        traverse = traverse->link;
      }
    }
  }

  hashFree(records);
  for(int i=0 ; i < N ; i++){
    free(arr[i]);
  }
  free(arr);
}

void test_hashlist_create(void){
  CNode list=newCliqueNode("test");
  HNode temp=newHashNode(list);
  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->top == list);
  TEST_ASSERT(temp->link == NULL);
  TEST_ASSERT(strcmp(temp->item,list->key)==0);
  cliqueFreeList(list);
  hashFreeList(temp);

}

void test_hashlist_insert(void){
  int N=20;
  char buffer[50];
  char **arr = malloc(sizeof(char *)* N);
  HNode list=NULL;
  CNode tmp=NULL;

  for(int i=0; i<N; i++){
    sprintf(buffer,"%d",i);
    arr[i] = malloc(strlen(buffer)+1);
    strcpy(arr[i],buffer);
    cliqueListInsert(&tmp,NULL,arr[i],arr[i]);
    hashListInsert(&list,tmp);
  }

  HNode traverse = list;
  for(int i=N-1; i>=0 ;i--){
    TEST_ASSERT(strcmp(traverse->item,arr[i]) == 0);
    TEST_ASSERT(traverse->top != NULL);
    traverse = traverse->link;
  }
  cliqueFreeList(tmp);
  hashFreeList(list);
  for(int i = 0 ; i < N ; i++)
    free(arr[i]);
  free(arr);
       
}

void test_hashlist_search(void){
  int N=20;
  char buffer[50];
  char **arr = malloc(sizeof(char *)* N);
  HNode list=NULL;
  CNode tmp=NULL;

  for(int i=0; i<N; i++){
    sprintf(buffer,"%d",i);
    arr[i] = malloc(strlen(buffer)+1);
    strcpy(arr[i],buffer);
    cliqueListInsert(&tmp,NULL,arr[i],arr[i]);
    hashListInsert(&list,tmp);
  }
  CNode node=tmp;
  for(int i = N-1 ; i >=0 ; i--){
    CNode found_node = hashListSearch(list, arr[i]);
    TEST_ASSERT(found_node == node);
    TEST_ASSERT(strcmp(found_node->key,arr[i])==0);
    node = node->right;
  }

  cliqueFreeList(tmp);
  hashFreeList(list);
  for(int i = 0 ; i < N ; i++)
  free(arr[i]);
  free(arr);
       
}

void test_bucket_create(void){
  pbucket temp = newBucket();

  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->info == NULL);
  TEST_ASSERT(temp->negative_relationships == 0);
  TEST_ASSERT(temp->arr == NULL);
  TEST_ASSERT(temp->arr_size == 0);
  TEST_ASSERT(temp->no_items == 0);

  bucketFree(temp);
}

void test_negative_relationships(void){
  pbucket test1 = newBucket();
  pbucket test2 = newBucket();

  negative_relationship(test1,test2);

  TEST_ASSERT(test1->arr != NULL);
  TEST_ASSERT(test1->arr_size != 0);
  TEST_ASSERT(test2->arr != NULL);
  TEST_ASSERT(test2->arr_size != 0);
  TEST_ASSERT(test1->arr[0] == test2);
  TEST_ASSERT(test2->arr[0] == test1);

  bucketFree(test1);
  bucketFree(test2);
}

void test_weight_create(void){
  weight_ptr temp = create_weights();

  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->b == 0);
  TEST_ASSERT(temp->w != NULL);
  for(int i = 0 ; i < 2 * vector_size() ; i++){
    TEST_ASSERT(temp->w[i] == 0);
  }

  free_weights(temp);
}

void test_initialize_index_vector(void){
  int* temp = initialize_index_vector();

  TEST_ASSERT(temp != NULL);
  for(int i = 0 ; i < 2 * vector_size() ; i++){
    TEST_ASSERT(temp[i] == -1);
  }

  free(temp);
}

void test_sparse_array(void){
  int* temp = malloc(sizeof(int) * vector_size() * 2);
  for(int i = 0 ; i < 2 * vector_size() ; i++){
    temp[i] = 0;
  }

  int size = 0;
  int* indexes = sparse_array(temp, &size,NULL,0,0);

  TEST_ASSERT(indexes == NULL);
  TEST_ASSERT(size == 0);

  for(int i = 0 ; i < 2 * vector_size() ; i++){
    if(i == 0){
      temp[i] = 1;
    }else{
      temp[i] = rand() % 2;
    }
  }

  indexes = sparse_array(temp, &size,NULL,0,0);
  TEST_ASSERT(indexes != NULL);
  TEST_ASSERT(size != 0);
  for(int i = 0 ; i < size ; i++){
    TEST_ASSERT(temp[indexes[i]] != 0);
  }

  free(indexes);
  free(temp);
}

void test_create_batch(void){
  batch_pointer temp = create_batch(100,NULL);

  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->batches != NULL);
  TEST_ASSERT(temp->batch_size == 100);
  for(int i = 0 ; i < temp->batch_size ; i++){
    TEST_ASSERT(temp->batches[i] == NULL);
  }
  TEST_ASSERT(temp->words == NULL);
  TEST_ASSERT(temp->successfull == 0);

  free(temp->batches);
  free(temp);
}

void test_create_full_set(void){
  full_set_ptr temp = create_full_set(100);

  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->test_size == 100 * 0.2);
  TEST_ASSERT(temp->validation_size == 100 * 0.2);
  TEST_ASSERT(temp->training_size == 100 - 100 * 0.2 - 100 * 0.2);
  TEST_ASSERT(temp->test_set != NULL);
  for(int i = 0 ; i < temp->test_size ; i++){
    TEST_ASSERT(temp->test_set[i] == NULL);
  }
  TEST_ASSERT(temp->validation_set != NULL);
  for(int i = 0 ; i < temp->validation_size ; i++){
    TEST_ASSERT(temp->validation_set[i] == NULL);
  }
  TEST_ASSERT(temp->training_set != NULL);
  for(int i = 0 ; i < temp->training_size ; i++){
    TEST_ASSERT(temp->training_set[i] == NULL);
  }

  free_full_set(temp);
}

void test_create_vectorized(void){
  full_set_ptr temp = create_full_set(1);
  temp->training_set[0] = malloc(sizeof(set));
  temp->training_set[0]->expected = 0;
  temp->training_set[0]->string1 = malloc(strlen("test1") + 1);
  strcpy(temp->training_set[0]->string1,"test1");
  temp->training_set[0]->string2 = malloc(strlen("test2") + 1);
  strcpy(temp->training_set[0]->string2,"test2");

  vset_ptr v_set = create_vectorized_set(0, temp->training_set,NULL);

  TEST_ASSERT(v_set != NULL);
  TEST_ASSERT(v_set->string1 != NULL);
  TEST_ASSERT(strcmp(temp->training_set[0]->string1, v_set->string1) == 0);
  TEST_ASSERT(v_set->string2 != NULL);
  TEST_ASSERT(strcmp(temp->training_set[0]->string2, v_set->string2) == 0);
  TEST_ASSERT(v_set->value == temp->training_set[0]->expected);
  TEST_ASSERT(v_set->x_vector != NULL);
  for(int i = 0 ; i < 2 * vector_size() ; i++){
    TEST_ASSERT(v_set->x_vector[i] == 0);
  }

  free_full_set(temp);
  free(v_set->string1);
  free(v_set->string2);
  free(v_set->x_vector);
  free(v_set);

}

void test_create_set(void){
  char* string1 = malloc(strlen("test1") + 1);
  strcpy(string1,"test1");
  char* string2 = malloc(strlen("test2") + 1);
  strcpy(string2,"test2");
  int x = rand() % 10;

  set_ptr temp = create_set(string1,string2,x);
  
  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->string1 != NULL);
  TEST_ASSERT(strcmp(temp->string1,string1) == 0);
  TEST_ASSERT(temp->string2 != NULL);
  TEST_ASSERT(strcmp(temp->string2,string2) == 0);
  TEST_ASSERT(temp->expected == x);

  free(temp->string1);
  free(temp->string2);
  free(temp);
  free(string1);
  free(string2);

}

void test_hashtable_BOW_initialize(void){
	ht_w *hash_table = ht_create(0);
	TEST_ASSERT(hash_table != NULL);
  TEST_ASSERT(hash_table->entries != NULL);

  	for(int i=0 ; i < TABLE_SIZE ; i++){
    		TEST_ASSERT(hash_table->entries[i] == NULL);
  	}

  	ht_free(hash_table,0);
}

void test_hashtable_BOW_insert(void){
  ht_w* hash_table = ht_create(0);

  int x = rand() % 10;

  entry_w* temp = ht_insert(hash_table,"test", "path", x);

  TEST_ASSERT(temp != NULL);
  for(int i = 0 ; i < TABLE_SIZE ; i++){
    if(hash_table->entries[i] != NULL){
      TEST_ASSERT(temp == hash_table->entries[i]);
    }
  }

  entry_w* test = ht_insert(hash_table,"test", "path", x);
  TEST_ASSERT(test == NULL);

  ht_free(hash_table,0);
  
}

void test_create_entry(void){
  entry_w* temp = create_entry("test","fullpath");
  
  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->word != NULL);
  TEST_ASSERT(temp->path != NULL);
  TEST_ASSERT(strcmp(temp->word,"test") == 0);
  TEST_ASSERT(strcmp(temp->path,"fullpath") == 0);
  TEST_ASSERT(temp->count_different_json == 1);
  TEST_ASSERT(temp->count_different_word == 0);
  TEST_ASSERT(temp->count_word == 1);
  TEST_ASSERT(temp->index == -1);
  TEST_ASSERT(temp->tf == 0);
  TEST_ASSERT(temp->tf_idf == 0);
  TEST_ASSERT(temp->idf == 0);
  TEST_ASSERT(temp->next == NULL);

  ht_entry_free(temp);
}

void test_update_entry(void){
  entry_w* temp = create_entry("test","fullpath");

  int x = rand() % 10 + 2;
  int y = rand() % 10;

  update_entry(temp, x, 0);

  TEST_ASSERT(temp->count_word == 2);
  TEST_ASSERT(temp->count_different_json == 2);
  
  update_entry(temp,0,y);

  TEST_ASSERT(temp->count_different_word == y);

  ht_entry_free(temp);
}

void test_copy_entry(void){
  entry_w* temp = create_entry("test","fullpath");

  int x = rand() % 10;

  entry_w* test = copy_entry(temp, x);
  
  TEST_ASSERT(test != NULL);
  TEST_ASSERT(test->word != NULL);
  TEST_ASSERT(strcmp(test->word, temp->word) == 0);
  TEST_ASSERT(test->path != NULL);
  TEST_ASSERT(strcmp(test->path, temp->path) == 0);
  TEST_ASSERT(test->count_different_json == temp->count_different_json);
  TEST_ASSERT(test->count_word == temp->count_word);
  TEST_ASSERT(test->tf == temp->tf);
  TEST_ASSERT(test->idf == temp->idf);
  TEST_ASSERT(test->tf_idf == temp->tf_idf);
  TEST_ASSERT(test->next == NULL);
  TEST_ASSERT(test->index == x);

  ht_entry_free(test);
  ht_entry_free(temp);

}

void test_ht_entry_insert(void){
  ht_w* ht = ht_create(10000);

  int x = rand() % 10;

  entry_w* temp = create_entry("test","fullpath");

  ht_entry_insert(ht, temp, x);
  
  TEST_ASSERT(ht_search(ht,temp->word) == x);

  ht_entry_insert(ht, temp, x);

  ht_free(ht,10000);
  ht_entry_free(temp);

}

void test_ht_search(void){
  ht_w* ht = ht_create(10000);

  int x = rand() % 10 + 1;

  entry_w* temp = create_entry("test","fullpath");

  ht_entry_insert(ht, temp, x);

  TEST_ASSERT(ht_search(ht,temp->word) != -1);
  TEST_ASSERT(ht_search(ht,"string") == -1);

  ht_entry_free(temp);
  ht_free(ht, 10000);
}

void test_queue_initialize(void){

  queue_ptr temp = queue_initialize();

  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->first == NULL);
  TEST_ASSERT(temp->last == NULL);
  TEST_ASSERT(temp->no_jobs == 0);

  free_queue(temp);

}

void test_create_queue_node(void){
  job_ptr test = create_new_job(NULL, NULL);

  QNode temp = create_queue_node(test);

  TEST_ASSERT(temp != NULL);
  TEST_ASSERT(temp->next == NULL);
  TEST_ASSERT(temp->prev == NULL);
  TEST_ASSERT(temp->work == test);

  free_queue_node(temp);
  free(test);

}

void test_insert_queue_node(void){
  
  queue_ptr temp = queue_initialize();
  job_ptr temp_job = create_new_job(NULL, NULL);
  temp->last = create_queue_node(temp_job);

  QNode x = temp->last;

  temp->last = insert_queue_node(temp->last, temp_job);

  TEST_ASSERT(temp->last != NULL);
  TEST_ASSERT(temp->last->prev == x);

  free_queue(temp);
  free_queue_node(x);
  free(temp_job);
}

void test_queue_push(void){
  queue_ptr temp = queue_initialize();
  job_ptr temp_job = create_new_job(NULL, NULL);

  queue_push(temp, temp_job);

  TEST_ASSERT(temp->first != NULL);
  TEST_ASSERT(temp->last != NULL);
  TEST_ASSERT(temp->first == temp->last);
  TEST_ASSERT(temp->no_jobs == 1);

  queue_push(temp, temp_job);

  TEST_ASSERT(temp->last != NULL);
  TEST_ASSERT(temp->last->prev == temp->first);
  TEST_ASSERT(temp->first->next == temp->last);
  TEST_ASSERT(temp->no_jobs == 2);

  free_queue(temp);
  free(temp_job);

}

void test_queue_pop(void){
  queue_ptr temp = queue_initialize();
  job_ptr temp_job = create_new_job(NULL, NULL);

  queue_push(temp, temp_job);

  QNode x = temp->first;

  QNode test = queue_pop(temp);

  TEST_ASSERT(test == x);
  TEST_ASSERT(queue_empty(temp) == 1);

  free_queue(temp);
  free_queue_node(test);
  free(temp_job);
}

void test_queue_empty(void){
  queue_ptr temp = NULL;

  TEST_ASSERT(queue_empty(temp) == 1);

  temp = queue_initialize();

  TEST_ASSERT(queue_empty(temp) == 1);

  job_ptr temp_job = create_new_job(NULL, NULL);

  queue_push(temp, temp_job);

  TEST_ASSERT(queue_empty(temp) == 0);

  free_queue(temp);
  free(temp_job);
}



TEST_LIST = {
  { "list_clique_create", test_clique_create},
	{ "list_clique_insert", test_clique_insert },
  { "list_clique_search", test_clique_search },
  { "list_record_create", test_record_create},
  { "list_record_insert", test_record_insert },
  { "list_insert", test_insert },
  { "hash_initialize", test_hash_initialize },
  { "hash_insert", test_hash_insert },
  { "hash_search", test_hash_search },
  { "list_hashlist_create", test_hashlist_create},
  { "list_hashlist_insert", test_hashlist_insert},
  { "list_hashlist_search", test_hashlist_search},
  { "hash_table_BOW_create", test_hashtable_BOW_initialize},
  { "bucket_create", test_bucket_create},
  { "negative_relationships", test_negative_relationships},
  { "weight_create", test_weight_create},
  { "initialize_index_vector", test_initialize_index_vector},
  { "sparse_array", test_sparse_array},
  { "create_batch", test_create_batch},
  { "create_full_set", test_create_full_set},
  { "create_vectorized_set", test_create_vectorized},
  { "create_set", test_create_set},
  { "hashtable_BOW_insert", test_hashtable_BOW_insert},
  { "hashtable_create_entry", test_create_entry},
  { "hashtable_update_entry", test_update_entry},
  { "hashtable_copy_entry", test_copy_entry},
  { "hashtable_entry_insert", test_ht_entry_insert},
  { "hashtable_search", test_ht_search},
  { "queue_initialize", test_queue_initialize},
  { "create_queue_node", test_create_queue_node},
  { "insert_queue_node", test_insert_queue_node},
  { "queue_push", test_queue_push},
  { "queue_pop", test_queue_pop},
  { "queue_empty", test_queue_empty},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
