#include <stdio.h>
#include <stdlib.h>
#include "prompt.h"

//Creates new node of type pcode
pcode newPostNode(SNode data){
	pcode temp = malloc(sizeof(postCode));

	temp->city = data->info->zip;
	temp->counter = 1;
	temp->postlist = newBucketNode(data);
	temp->right = NULL;
	temp->left = NULL;

	return temp;
}

//Frees all alocated data used by list of pcode
void postFreeList(pcode head){

	if(head == NULL) return;

	pcode temp;

	while(head != NULL){
		temp = head;
		head = head->right;
		bucketFreeList(temp->postlist);
		temp->right = NULL;
		temp->left = NULL;
		free(temp);
	}
}

//Searches the list to find if asked zip code is in list and returns the node or NULL if not found
pcode postListSearch(pcode head, int place){
	if(head == NULL) return NULL;

	if(head->city == place) return head;
	else{
		pcode temp = head->right;

		while(temp != NULL){
			if(temp->city == place) return temp;

			temp = temp->right;
		}
	}

	return NULL;
}

//Deletes a node from the bucket list of postList and if there are no more buckets deletes the node of postlist
void postListDelete(pcode *head, int ID, int city){
	if(*head == NULL) return;

	//Searches the asked zip and returns the node
	pcode temp = postListSearch((*head), city);
	if(temp == NULL) return;

	//Deletes node from bucket list and reduces counter by one
	bucketListDelete(&(temp->postlist), bucketListSearch(temp->postlist, ID));
	temp->counter--;

	//Checks if node from which we deleted was the head of the list
	if(temp == *head){
		temp = (*head)->right;
		if(temp == NULL) return;
		
		//If the head of the list no longer has the most values swap places with the next node
		if(temp->counter > (*head)->counter){
			temp->left = NULL;
			if(temp->right != NULL) temp->right->left = *head;
			(*head)->right = temp->right;
			temp->right = *head;
			*head = temp;
			return;
		}
		temp = (*head)->right;
	}

	//Check if there no more values in the node ,swap it and delete the node
	if(temp->counter == 0){
		if(temp->right != NULL && temp->left != NULL) temp->right->left = temp->left;
		else if(temp->left == NULL){
			temp = (*head)->right;
			if(temp == NULL) return;
			temp->left = NULL;
			if(temp->right != NULL) temp->right->left = *head;
			(*head)->right = temp->right;
			temp->right = *head;
			*head = temp;
		}else{
			temp->left->right = NULL;
		}

		bucketFreeList(temp->postlist);
		free(temp);
		return;
	}

	pcode current = (temp->right);
	pcode previous = temp;

	if(current == NULL) return;

	//Find where node should be after reduced value
	while((current!=NULL) && (current->counter > previous->counter)){
		current->left = previous->left;
		if(previous->left != NULL) previous->left->right = current;
		previous->left = current;
		if(current->right != NULL) current->right->left = previous;
		previous->right = current->right;
		current->right = previous;
		
		current = previous->right;
	}

	return;

}

//Inserts a node to post list
void postListInsert(pcode *head, SNode data){
	//if no nodes in list
	if(*head == NULL){
		*head = newPostNode(data);
	}else{
		//if city to insert is the same as the head of the list insert and increase counter
		if(data->info->zip == (*head)->city){
			bucketListInsert(&((*head)->postlist), data);
			(*head)->counter++;
		}else{
			//Check the second node
			if((*head)->right != NULL){
				//Insert if city is the same
				if((*head)->right->city == data->info->zip){
					bucketListInsert(&((*head)->right->postlist), data);
					(*head)->right->counter++;

					//check if there are more nodes in second node than in head and swap
					if((*head)->right->counter > (*head)->counter){
						pcode temp = (*head)->right;

						temp->left = NULL;
						if(temp->right != NULL) temp->right->left = *head;
						(*head)->right = temp->right;
						(*head)->left = temp;
						temp->right = *head;
						*head = temp;
					}

					return;
				}
			}

			pcode traverse = (*head)->right;
			pcode previous = *head;

			//else traverse the rest of the list until you find the asked city
			while(traverse != NULL){
				if(traverse==(*head)->right){
					previous = traverse;
					traverse = traverse->right;
					continue;
				}
				//if city found insert
				if(traverse->city == data->info->zip){
					bucketListInsert(&(traverse->postlist), data);

					traverse->counter++;
					pcode current = traverse->left;
					pcode prev = traverse;

					//Check if node must go higher
					while((current != NULL) && (prev->counter > current->counter)){
						//If we reach top node so that we can change head of list
						if(current->left != NULL){
							current->right = prev->right;
							if(prev->right != NULL) prev->right->left = current;					
							prev->left = current->left;
							prev->right = current;
							if(current->left != NULL) current->left->right = prev;
							current->left = prev;					
	
						}else{
							prev->left = NULL;
							if(prev->right != NULL)  prev->right->left = *head;
							(*head)->right = prev->right;
							(*head)->left = prev;
							prev->right = *head;
							*head = prev;
						}

						current = prev->left;
					}

					return;
				}

				previous = traverse;
				traverse = traverse->right;

			}

			//if node not found previous will always be the last node so we insert new node there
			previous->right = newPostNode(data);
			previous->right->left = previous;
		}
	}
}