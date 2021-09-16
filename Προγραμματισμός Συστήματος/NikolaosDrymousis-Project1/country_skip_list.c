#include "header.h"

//Find and delete an item from the skip lists
int country_skip_lists_search_delete(SList* head, int height, char* item){
  if(head == NULL) return 0;

  for(int i = height - 1 ; i >= 0 ; i--){
    if(country_skip_list_search_delete(head[i], item)) return 1;
  }

  printf("Record does not exist\n");

  return 0;
}

//Find a delete an item from a skip list
int country_skip_list_search_delete(SList head, char* item){
  if(head == NULL) return 0;

  SList traverse = head;

  while(traverse->right){
    //If found
    if(compareDate(traverse->skip_date, item) == 0){

      //Go lower at the skip list and delete
      skip_list_delete_below(traverse, traverse->right);
      return 1;

    }else if(compareDate(traverse->skip_date, item) == -1){
      //else if item date is bigger than that of the node
      //if the the next node's date is bigger than the item date go lower at the skip lists
      if(traverse->right != NULL && compareDate(traverse->skip_date, item) == 1){
        //If level is 1 search list
        if(traverse->level == 1) return country_list_delete(traverse->node, item);
        //else travel lower at the skip list
        return country_skip_list_search_delete(traverse->lower, item);
      }
    }

    traverse = traverse->right;
  }

  //compare last node
  if(compareDate(traverse->skip_date, item) == -1){
    if(traverse->level == 1) return country_list_delete(traverse->node, item);
    return country_skip_list_search_delete(traverse->lower, item);
  }

  //if last node and level is 1
  if(traverse->level == 1) return country_list_delete(traverse->node, item);

  return 0;
}

//Search skip lists and return found node or NULL
LNode country_skip_lists_search(SList* head, int height, char* item, int flag){
  if(head == NULL) return NULL;

  for(int i = height - 1 ; i >= 0 ; i--){
    //Search skip list by level
    LNode temp = country_skip_list_search(head[i], item, flag);
    if(temp) return temp;
  }


  return NULL;
}

//Seach skip list and return node or NULL
LNode country_skip_list_search(SList head, char* item, int flag){
  if(head == NULL) return NULL;

  SList traverse = head;

  //Same logic as search delete but just return node
  while(traverse){
    if(compareDate(traverse->skip_date, item) == 0){

      SList current = traverse;
      while(current->lower){
        current = current->lower;
      }

      return current->node;
    }else if(compareDate(traverse->skip_date, item) == -1){
      if(traverse->right != NULL && compareDate(traverse->skip_date, item) == 1){
        if(traverse->level == 1) return country_list_node_search(traverse->node, item, flag);
        return country_skip_list_search(traverse->lower, item, flag);
      }

      traverse = traverse->right;
    }else{
      return NULL;
    }
  }

  return NULL;
}

//Search skip lists to insert node at correct position
void country_skip_lists_search_insert(SList* levels, LNode head, int height, char** item){
  if(levels == NULL){
    country_list_insert(&head, item);
    return;
  }

  for(int i = height-1 ; i >= 0 ; i--){
    if(country_skip_list_search_insert(levels[i], item)){
      return;
    }
  }
}

//Search skip list to insert node at correct position
int country_skip_list_search_insert(SList head, char** item){
  if(head == NULL) return 0;

  SList traverse = head;

  while(traverse){
    //If node date > item date return
    if(compareDate(traverse->skip_date, item[vac_date]) == 1){
      return 0;
    }else if(compareDate(traverse->skip_date, item[vac_date]) == -1){
      //Same logic as country_skip_list_delete but this time insert
      if(traverse->right != NULL && compareDate(traverse->skip_date, item[vac_date]) >= 0){
        if(traverse->level == 1) return country_list_node_search_insert(traverse, traverse->node, item);
        return country_skip_list_search_insert(traverse->lower, item);
      }
    }else if(traverse->right == NULL){
      //If last node of skip list go lower from there
      if(traverse->level == 1) return country_list_node_search_insert(traverse, traverse->node, item);
      int x = country_skip_list_search_insert(traverse->lower, item);

      //If skip list level > 2 see if new node height is going to be increased
      if(x && traverse->level > 2){
        traverse->right = skip_increase_height(traverse->lower);
      }

      return x;
    }


    if(traverse->right != NULL && traverse->level != 1) traverse = traverse->right;
    else if(traverse->level == 1 && traverse->right == NULL){
      //If last node of skip list and level is 1 go lower at the list to search and insert
      return country_list_node_search_insert(traverse, traverse->node, item);
    }
    else traverse = traverse->right;
  }

  return 0;
}

//Insert at list
void country_list_insert(LNode *head, char** item){
  //If list is empty create new node and insert at head of list
  if(*head == NULL){
    *head = new_list_node(item);
    return;
  }

  LNode temp = new_list_node(item);

  //If head != NULL and new node date is smaller than head node date swap
  if(compareDate((*head)->data->date, temp->data->date) == 1){
    temp->next = *head;
    *head = temp;
  }else if((*head)->next == NULL){
    //Else if head next is NULL insert node at second place
    (*head)->next = temp;
  }else{

    //Else traverse the list to find the correct node to be inserted
    LNode previous = *head;
    LNode traverse = (*head)->next;

    while(traverse->next != NULL){
      if(compareDate(traverse->data->date, temp->data->date) == 1){
        previous->next = temp;
        temp->next = traverse;

        return;
      }

      previous = traverse;
      traverse = traverse->next;
    }

    traverse->next = temp;

  }

}

//Search the list to find the correct place to insert and maybe add new node to skip list
int country_list_node_search_insert(SList top, LNode head, char** item){
  if(head == NULL) return 0;

  LNode traverse = head;

  while(traverse->next){
    if(compareDate(traverse->next->data->date, item[vac_date]) == 1){
      LNode temp = new_list_node(item);

      temp->next = traverse->next;
      traverse->next = temp;
      
      //Check to see if we will increase skip list height
      if(rand() % 2){
        //If skip list node was the last insert new skip node at the end
        if(top->right == NULL){
          top->right = new_skip_node(temp, NULL);
        }else if(compareDate(top->right->skip_date, temp->next->data->date) == 0){
          //Else if next node of skip list is connected to the previous
          SList new_node = new_skip_node(temp, NULL);

          new_node->right = top->right;
          top->right = new_node;

        }
      }

      return 1;
    }

    traverse = traverse->next;
  }

  //new country list node
  traverse->next = new_list_node(item);

  //Increase skip height
  if(rand() % 2){
    if(top->right == NULL){
      top->right = new_skip_node(traverse->next, NULL);
    }
  }

  return 1;
}

//Search and return a list node
LNode country_list_node_search(LNode head, char* item, int flag){
  if(head == NULL) return NULL;

  //If node is at the start of the list
  if(compareDate(head->data->date, item) == 0) return head;
  if(compareDate(head->data->date, item) == 1 && flag) return NULL;

  LNode traverse = head->next;
  LNode prev = head;

  while (traverse){
    //if asked date is found return
    if(compareDate(traverse->data->date, item) == 0) return traverse;

    //Else if asked date is not found return the next closest to it
    if(compareDate(traverse->data->date, item) == 1 && flag) return traverse;

    //Else if asked date is not found return the previous closest to it
    if(compareDate(traverse->data->date, item) == 1 && !flag) return prev;

    prev = traverse;
    traverse = traverse->next;
  }

  return prev;
}

//Find and delete a list node
int country_list_delete(LNode head, char* target){
  if(head == NULL) return 0;

  LNode traverse = head;

  while(traverse){
    if(compareDate(traverse->next->data->date, target) == 0){
      LNode temp = traverse->next;

      traverse->next = temp->next;
      temp->next = NULL;
      list_free(temp);
      return 1;
    }

    traverse = traverse->next;
  }

  return 0;
}