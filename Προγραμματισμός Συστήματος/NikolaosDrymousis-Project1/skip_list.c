#include "header.h"

//Creates a new skip list node
SList new_skip_node(LNode item, SList below){

  SList temp = malloc(sizeof(skip_list));

  //If item is given we are at level 1 of skip list
  if(item != NULL){ 
    temp->skip_id = item->data->citizenID;
    if(item->data->date != NULL){
      temp->skip_date = malloc(strlen(item->data->date) + 1);
      strcpy(temp->skip_date, item->data->date);
    }else{
      temp->skip_date = NULL;
    }
    temp->level = 1;
    temp->node = item;
    temp->lower = NULL;
    temp->right = NULL;
    temp->above = NULL;
  }else if(below != NULL){
    //Else if below is given we are at level 2 and above of skip list
    temp->skip_id = below->skip_id;
    if(below->skip_date != NULL){
      temp->skip_date = malloc(strlen(below->skip_date) + 1);
      strcpy(temp->skip_date, below->skip_date);
    }else{
      temp->skip_date = NULL;
    }
    temp->level = below->level + 1;
    temp->node = NULL;
    temp->lower = below;
    temp->right = NULL;
    temp->above = NULL;
    below->above = temp;
  }else{
    free(temp);
    temp = NULL;
  }

  return temp;

}

//Create the first level of skip list
SList new_skip_list(LNode head){

  //first node of list is also first node of skip list
  SList temp = new_skip_node(head, NULL);

  LNode traverse = head->next;
  SList current = temp;

  while(traverse != NULL){

    //Randomly add to skip list
    if(rand() % 2){
      current->right = new_skip_node(traverse, NULL);
      current = current->right;
    }

    traverse = traverse->next;

  }

  return temp;

}

//Increase the height of skip list using the lower level (item)
SList skip_increase_height(SList item){

  SList temp = new_skip_node(NULL, item);

  SList traverse = item->right;
  
  SList current = temp;

  while(traverse != NULL){

    int x = rand() % 2;

    if(x){
      current->right = new_skip_node(NULL, traverse);
      current = current->right;
    }

    traverse = traverse->right;

  }

  return temp;

}

//Search skip lists to find and delete record with id = item
int skip_lists_search_delete(SList* head, int height, int item){
  if(head == NULL) return 0;

  //begin from highest level
  for(int i = height - 1 ; i >= 0 ; i--){
    if(skip_list_search_delete(head[i], item)) return 1;
  }

  printf("Record does not exist\n");

  return 0;
}

//Search a skip list to find and delete record with id = item
int skip_list_search_delete(SList head, int item){
  if(head == NULL) return 0;

  SList traverse = head;

  while(traverse->right){
    //if node is found traverse lower and delete all skip nodes lower than this one
    if(traverse->right->skip_id == item){
      skip_list_delete_below(traverse, traverse->right);
      return 1;

    }else if(traverse->skip_id < item){
      //if the next id is bigger go lower at the skip list
      if(traverse->right->skip_id > item){
        //if skip list level is 1 search the list
        if(traverse->level == 1) return list_delete(traverse->node, item);
        return skip_list_search_delete(traverse->lower, item);
      }
    }

    traverse = traverse->right;
  }

  //if last node id is smaller search lower
  if(traverse->skip_id < item){
    if(traverse->level == 1) return list_delete(traverse->node, item);
    return skip_list_search_delete(traverse->lower, item);
  }

  if(traverse->level == 1) return list_delete(traverse->node, item);

  return 0;
}

//delete all skip list nodes with found id and fix transitivity issues
void skip_list_delete_below(SList previous, SList current){

  SList traverse = previous;

  while(traverse->right){
    if(traverse->right == current){
      traverse->right = current->right;
      if(traverse->level == 1){
        list_delete(previous->node, current->skip_id);
      }else{
        skip_list_delete_below(traverse, current->lower);
      }  
      break;
    }

    traverse = traverse->right;
  }

  current->right = NULL;
  skip_list_free(current);

}

//Search skip lists for node with id = item and return it
LNode skip_lists_search(SList* head, int height, int item){
  if(head == NULL) return NULL;

  for(int i = height - 1 ; i >= 0 ; i--){
    LNode temp = skip_list_search(head[i], item);
    if(temp) return temp;
  }


  return NULL;
}

//Search skip list for node with id = item and return it
LNode skip_list_search(SList head, int item){
  if(head == NULL) return 0;

  SList traverse = head;

  while(traverse){
    //If found go to the lowest level and return list node
    if(traverse->skip_id == item){

      SList current = traverse;
      while(current->lower){
        current = current->lower;
      }

      return current->node;
    }else if(traverse->skip_id < item){
      //Same logic as delete but this time return found node
      if(traverse->right != NULL){
        if(traverse->right->skip_id > item){
          if(traverse->level == 1) return list_node_search(traverse->node, item);
          return skip_list_search(traverse->lower, item);
        }
      }else if(traverse->right == NULL){
        if(traverse->level == 1) return list_node_search(traverse->node, item);
        return skip_list_search(traverse->lower, item);
      }

      traverse = traverse->right;
    }else{
      return NULL;
    }
  }

  return NULL;
}

//Search skip lists to find the correct location to add the node
void skip_lists_search_insert(SList* levels, LNode head, int height, char** item){
  if(levels == NULL){
    list_insert(&head, item);
    return;
  }

  for(int i = height-1 ; i >= 0 ; i--){
    if(skip_list_search_insert(levels[i], item)){
      return;
    }
  }
}

//Search skip list to find the correct location to add the node
int skip_list_search_insert(SList head, char** item){
  if(head == NULL) return 0;

  SList traverse = head;

  while(traverse){
    //if id is bigger return
    if(traverse->skip_id > atoi(item[citizen])){
      return 0;
    }else if(traverse->skip_id < atoi(item[citizen])){
      //Same logic as delete but this time insert it
      if(traverse->right != NULL && traverse->right->skip_id > atoi(item[citizen])){
        if(traverse->level == 1) return list_node_search_insert(traverse, traverse->node, item);
        return skip_list_search_insert(traverse->lower, item);
      }else if(traverse->right == NULL){
        //if last node and item > than current node id go lower
        if(traverse->level == 1) return list_node_search_insert(traverse, traverse->node, item);
        int x = skip_list_search_insert(traverse->lower, item);

        if(x && traverse->level >= 2){
          //Increase height of skip list after successfull insertion
          traverse->right = skip_increase_height(traverse->lower);
        }

        return x;
      }
    }

    if(traverse->right != NULL && traverse->level != 1) traverse = traverse->right;
    else if(traverse->level == 1 && traverse->right == NULL){
      return list_node_search_insert(traverse, traverse->node, item);
    }
    else traverse = traverse->right;
  }

  return 0;
}

//free skip lists
void skip_lists_free(SList* head, int height){
  if(head == NULL) return;

  for(int i = 0 ; i < height ; i++){
    skip_list_free(head[i]);
  }

  free(head);
}

//free skip list
void skip_list_free(SList head){
  if(head == NULL) return;

  skip_list_free(head->right);

  free(head->skip_date);
  free(head);
}

//Print skip lists(NO use currently)
void skip_lists_print(SList* head, int height){

  for(int i = height-1 ; i >= 0 ; i--){
    skip_list_print(head[i]);
  }
}

//Print skip list(NO use currently)
void skip_list_print(SList head){
  if(head == NULL) return;

  SList traverse = head;
  printf("\n\n\n\nlevel: %d\n\n\n\n",traverse->level);

  while(traverse){
    printf("skip id = %d \n", traverse->skip_id);

    skip_list_below(traverse->lower);

    traverse = traverse->right;
  }

}

//Print lower skip list(NO use currently)
void skip_list_below(SList head){
  if(head == NULL) return;

  if(head->level != 1) skip_list_below(head->lower);
  else{
    printf("citizen id = %d\n",head->node->data->citizenID);
    printf("virus id = %s\n",head->node->data->virusName);
  }
}

//Unused logarithmic function(earlier stages)
unsigned int Log2n(unsigned int n){ 
    return (n > 1) ? 1 + Log2n(n / 2) : 0;
}