#include <stdio.h>
#include "prompt.h"

//Implementation of postal code
void postalcode(pcode head, int rank){
	if(listEmpty(head)){
		printf("No students are enrolled\n");
		return;
	}

	pcode temp = head;
	int flag = 0;

	//Check if asked rank exist (nodes are less than rank)
	for(int i=1 ; i<rank ; i++){
		if(temp == NULL){
			flag = 1;
			break;
		}
		temp=temp->right;
	}

	//Unused code
	/*if(temp!=NULL && temp->counter == 0){
		temp = temp->right;
		if(temp == NULL){
			printf("No students are enrolled\n");
			return;
		}
		while(temp != NULL && temp->counter == 0){
			temp = temp->right;
			if(temp == NULL){
				printf("No students are enrolled\n");
				return;
			}
		}
	}*/

	//if nodes are less than rank or node doesnt exist
	if(flag || (temp == NULL)){
		printf("No students are enrolled\n");
		return;
	}else{
		//Check if nodes next to the asked node have the same rank(if their count is the same they should be the same rank)
		if(temp->right != NULL){
			if(temp->left != NULL){
				if((temp->counter != temp->right->counter) && (temp->counter != temp->left->counter)){
					printf("(%d) is %d most popular\n", temp->city, rank);
					//printf("counter : %d",temp->counter);
					return;
				}
			}else if(temp->counter != temp->right->counter){
				printf("(%d) is %d most popular\n", temp->city, rank);
				//printf("counter : %d",temp->counter);
				return;
			}
		}else{
			if(temp->left != NULL){
				if(temp->counter != temp->left->counter){
					printf("(%d) is %d most popular\n", temp->city, rank);
					//printf("counter : %d",temp->counter);
					return;
				}
			}
		}
		
	}

	//Use these flags to check neighbors of asked node
	int flag1 = 1;
	int flag2 = 1;
	int flag3 = 1;

	pcode prev = temp->left;
	if(prev == NULL) flag1 = 0;
	pcode front = temp->right;
	if(front == NULL) flag2 = 0;
	if(flag1 == 0 && flag2 == 0) flag3 = 0;
	
	//while there are still neighbours with the same rank continue
	while(flag3){
		//if there are nodes on the right
		if(flag2){
			if(front->counter == temp->counter){
				printf("(%d) ", front->city);
				//printf("counter : %d",front->counter);
				front = front->right;
				//if no more nodes
				if(front == NULL) flag2 = 0;
			}else{
				//if rank is less than asked rank
				flag2 = 0;
			}
		}
		
		//if there are nodes on the left
		if(flag1){
			if(prev->counter == temp->counter){
				printf("(%d) ", prev->city);
				//printf("counter : %d",prev->counter);
				prev = prev->left;
				//if no more nodes
				if(prev == NULL) flag1 = 0;
			}else{
				//if rank is greater than asked rank
				flag1 = 0;
			}
		}

		//if there are no more neighbours with same rank
		if(flag1 == 0 && flag2 == 0) flag3 = 0;
	}

	printf("(%d) ", temp->city);
	//printf("counter : %d",temp->counter);
	printf("are %d most popular\n", rank);
	return;
}