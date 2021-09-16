#include "header.h"

//Create and return record info
info_ptr new_info(char** item){

  info_ptr temp = malloc(sizeof(info));

  temp->age = atoi(item[years]);
  
  temp->citizenID = atoi(item[citizen]);
  
  temp->firstName = malloc(strlen(item[fname]) + 1);
  strcpy(temp->firstName, item[fname]);

  temp->lastName = malloc(strlen(item[lname]) + 1);
  strcpy(temp->lastName, item[lname]);

  temp->country = malloc(strlen(item[location]) + 1);
  strcpy(temp->country, item[location]);

  temp->virusName = malloc(strlen(item[virus_id]) + 1);
  strcpy(temp->virusName, item[virus_id]);

  temp->vaccine = malloc(strlen(item[yes_no]) + 1);
  strcpy(temp->vaccine, item[yes_no]);

  if(!strcmp(item[yes_no], "YES")){

    temp->date = malloc(strlen(item[vac_date]) + 1);
    strcpy(temp->date, item[vac_date]);

  }else{
    temp->date = NULL;
  }

  return temp;

}

//Create and return new list node
LNode new_list_node(char** item){

  LNode temp = malloc(sizeof(list));

  temp->data = new_info(item);
  temp->level = 0;
  temp->next = NULL;

  return temp;
}

//Insert node at ascending order
void list_insert(LNode *head, char** item){

  //If null create and insert node
  if(*head == NULL){
    *head = new_list_node(item);
    return;
  }

  LNode temp = new_list_node(item);

  //if new node id smaller than head id make the new node head of the list
  if((*head)->data->citizenID > temp->data->citizenID){
    temp->next = *head;
    *head = temp;
  }else if((*head)->next == NULL){
    //else if only one node add new node next to the head
    (*head)->next = temp;
  }else{
    //else search correct position to place new node
    LNode previous = *head;
    LNode traverse = (*head)->next;

    while(traverse->next != NULL){
      if(traverse->data->citizenID > temp->data->citizenID){
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

//Called from skip lists and possibly increases node height
int list_node_search_insert(SList top, LNode head, char** item){
  if(head == NULL) return 0;

  LNode traverse = head;

  while(traverse->next){
    //Insert at correct position
    if(traverse->next->data->citizenID > atoi(item[citizen])){
      LNode temp = new_list_node(item);

      temp->next = traverse->next;
      traverse->next = temp;

      //Randomly choose if node will become a skip node
      if(rand() % 2){
        if(top->right == NULL){
          top->right = new_skip_node(temp, NULL);

        }else if(top->right->skip_id == temp->next->data->citizenID){
          SList new_node = new_skip_node(temp, NULL);

          new_node->right = top->right;
          top->right = new_node;

        }
      }

      return 1;
    }

    traverse = traverse->next;
  }

  traverse->next = new_list_node(item);

  if(rand() % 2){
    if(top->right == NULL){
      top->right = new_skip_node(traverse->next, NULL);
    }
  }

  return 1;
}

//Search list to find item
int list_search(LNode head, int item){
  if(head == NULL) return 0;

  if(head->data->citizenID == item) return 1;

  return list_search(head->next, item);
}

//Used by skip lists and returns found node
LNode list_node_search(LNode head, int item){
  if(head == NULL) return NULL;

  if(head->data->citizenID == item) return head;

  LNode traverse = head->next;

  while (traverse){
    if(traverse->data->citizenID == item) return traverse;

    if(traverse->data->citizenID > item) return NULL;

    traverse = traverse->next;
  }

  return NULL;
  
}

//Prints the list(NO use currently)
void list_print(LNode head){
  if(head == NULL) return;

  LNode traverse = head;

  while(traverse){
    printf("%d %s %s %s %d\n",traverse->data->citizenID, traverse->data->firstName, traverse->data->lastName, traverse->data->country, traverse->data->age);
  
    traverse = traverse->next;
  }
  
}

//Frees the list
void list_free(LNode head){
  if(head == NULL) return;

  list_free(head->next);

  info_free(head->data);
  free(head);

}

//Free record information
void info_free(info_ptr item){
  free(item->country);
  free(item->date);
  free(item->firstName);
  free(item->lastName);
  free(item->vaccine);
  free(item->virusName);
  free(item);
}