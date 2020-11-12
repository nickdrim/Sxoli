#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "help.h"
#include "binary_tree.h"

/////////////////////////  AVL TREE FUNCTIONS /////////////////////////////////

int max(int a, int b) { 
    return (a > b)? a : b; 
} 

int getBalance(BTNode node){ 
    if (node == NULL) 
        return 0; 
    return height(node->left) - height(node->right); 
}

int height(BTNode node){ 
    if (node == NULL) 
        return 0; 
    return node->height; 
}

BTNode rightRotate(BTNode node){
    BTNode x = node->left; 
    BTNode T2 = x->right; 
  
    // Perform rotation 
    x->right = node; 
    node->left = T2; 
  
    // Update heights 
    node->height = max(height(node->left), height(node->right))+1; 
    x->height = max(height(x->left), height(x->right))+1; 
  
    // Return new root 
    return x; 
}

BTNode leftRotate(BTNode node){ 
    BTNode y = node->right; 
    BTNode T2 = y->left; 
  
    // Perform rotation 
    y->left = node; 
    node->right = T2; 
  
    //  Update heights 
    node->height = max(height(node->left), height(node->right))+1; 
    y->height = max(height(y->left), height(y->right))+1; 

    // Return new root 
    return y; 
} 

BTNode BTinsert(BTNode root , LNode temp){

	if(root==NULL){
		root = malloc(sizeof(tree));
        root->date = malloc(strlen(temp->record->entryDate)+1);
        strcpy(root->date , temp->record->entryDate);
		root->data = temp->record;
        root->height = 1;
		root->left = NULL;
		root->right = NULL;
		return root;
	}

	if(compareDate(root->data->entryDate , temp->record->entryDate) == 1){
		  root->left = BTinsert(root->left , temp);
	}
	else{
		  root->right = BTinsert(root->right , temp);
    }


    root->height = 1 + max(height(root->left),height(root->right));

	int balance = getBalance(root);

	if (balance > 1 && compareDate(root->data->entryDate , temp->record->entryDate)) 
        return rightRotate(root); 
  
    // Right Right Case 
    if (balance < -1 && compareDate(root->data->entryDate , temp->record->entryDate)!=1)
        return leftRotate(root); 
  
    // Left Right Case 
    if (balance > 1 && !compareDate(root->data->entryDate , temp->record->entryDate) !=1) 
    { 
        root->left =  leftRotate(root->left); 
        return rightRotate(root); 
    } 
  
    // Right Left Case 
    if (balance < -1 && compareDate(root->data->entryDate , temp->record->entryDate)) 
    { 
        root->right = rightRotate(root->right); 
        return leftRotate(root); 
    } 
  	
    /* return the (unchanged) root pointer */
    return root; 

}

int BTinfected(BTNode root){

    int size = 0;
    
    if(!strcmp(root->data->exitDate , "-")) size++;

    if(root->left!=NULL) size += BTinfected(root->left);
    if(root->right!=NULL) size += BTinfected(root->right);

    return size;
}

void BTdestroy(BTNode node){

    if(node == NULL) return;

    if(node->left!=NULL) BTdestroy(node->left);
    if(node->right!=NULL) BTdestroy(node->right);

    free(node->date);

    node->data=NULL;
    node->left=NULL;
    node->right=NULL;

    free(node);

}

/////////////////////////////// END OF AVL FUNCTIONS ////////////////////////////////

////////////////////////////// HELPFUL FUNCTIONS FOR SIZE OF TREES ////////////////////////////////////////


//Returns Full size of tree
int getSize(BTNode root){

	int size = 0;
    if(root!=NULL) size++;
	if(root->left!=NULL){
		size += getSize(root->left);
	}
	if(root->right!=NULL){
		size += getSize(root->right);
	}

	return size;

}

//Returns number of nodes that are within range of dates 
int getSizeDates(BTNode root , char *start , char *end){

    if(root == NULL) return 0;

	int size = 0;
    if(compareDate(root->data->entryDate , start) == 0 && compareDate(root->data->entryDate , end) == -1){
        size++;
    }else if(compareDate(root->data->entryDate , start) == 0 && compareDate(root->data->entryDate , end) == 0){
        size++;
    }else if(compareDate(root->data->entryDate , start) == 1 && compareDate(root->data->entryDate , end) == -1){
        size++;
    }else if(compareDate(root->data->entryDate , start) == 1 && compareDate(root->data->entryDate , end) == 0){
        size++;; 
    }

    size += getSizeDates(root->right , start , end);
    size += getSizeDates(root->left , start , end); 

    return size;
}

//Returns number of nodes that have the same country as place
int getSizeCountry(BTNode root , char *start , char *end , char* place){

    if(root == NULL) return 0;
   
    int size = 0;

    if(!strcmp(root->data->country , place)){
        if(start!=NULL){
            if(compareDate(root->date , start) == 0 && compareDate(root->date , end) == -1){
                size++;
            }else if(compareDate(root->date , start) == 0 && compareDate(root->date , end) == 0){
                size++;
            }else if(compareDate(root->date , start) == 1 && compareDate(root->date , end) == -1){
                size++;
            }else if(compareDate(root->date , start) == 1 && compareDate(root->date , end) == 0){
                size++;
            }
        
        }else{
            size++;
        }
    }

    size += getSizeCountry(root->right , start , end , place);
    size += getSizeCountry(root->left , start , end , place);

    return size;
    
}

//Returns number of nodes that have the same diseases as virusName
int getSizeDisease(BTNode root , char *start , char *end , char* virusName){

    if(root == NULL) return 0;
    
    int size = 0;
    
    if(!strcmp(root->data->diseaseID , virusName)){
        if(start!=NULL){
            if(compareDate(root->date , start) == 0 && compareDate(root->date , end) == -1){
                size++;
            }else if(compareDate(root->date , start) == 0 && compareDate(root->date , end) == 0){
                size++;
            }else if(compareDate(root->date , start) == 1 && compareDate(root->date , end) == -1){
                size++;
            }else if(compareDate(root->date , start) == 1 && compareDate(root->date , end) == 0){
                size++;
            }
        
        }else{
            size++;
        }
    }

    size += getSizeDisease(root->right , start , end , virusName);
    size += getSizeDisease(root->left , start , end , virusName);

    return size;

}

////////////////////////////////////////// HEAP FUNCTIONS ///////////////////////////////////////////////////////////

//returns size of heap
int heapSize(HPTree root){

    int size = 0;
    if(root == NULL) return size;

    size++;
    if(root->left !=NULL) heapSize(root->left);
    if(root->right !=NULL) heapSize(root->right);

    return size;
}

//Very helpfull in many cases with heap read PDF
HPTree heapLevelOrder(HPTree root , int *i){

    if(root == NULL) return NULL;
   
    QHead q = createQueue();
    enQueue(q , root);
    (*i)--;

    if(*i == 0){
        deQueue(q);
        free(q);
        return root;
    }

    HPTree temp = NULL;

    while(*i > 0){
        temp = getFront(q);
        if(temp->left != NULL) enQueue(q , temp->left);
        (*i)--;
        if((*i) == 0){
            while(q->front!=NULL) deQueue(q);
            free(q);
            return temp->left;
        }
        if(temp->right!= NULL) enQueue(q , temp->right);
        (*i)--;
        if((*i) == 0){
            while(q->front!=NULL) deQueue(q);
            free(q);
            return temp->right;
        }
        deQueue(q);
    }

    while(q->front!=NULL) deQueue(q);
    free(q);

    return temp;
   

}

//Use if you want to see heap. Not in programm or in comments
void printHeap(HPTree heap){

    if(heap == NULL) return;

    static int i=1;

    printf("add = %p , name = %s , no = %d\n", heap , heap->Name , heap->no);
    //printf("ladd = %p , radd = %p\n", heap->left , heap->right);

    

    if(heap->left) printHeap(heap->left);
    if(heap->right) printHeap(heap->right);
}

//Gets height of heap
int maxDepth(HPTree heap){
   
    if(heap == NULL) return 0;

    int ldepth = maxDepth(heap->left);
    int rdepth = maxDepth(heap->right);

    return max(ldepth , rdepth) + 1;
}

//Checks for max heap property
int checkHeap(HPTree heap){
    if(heap == NULL) return 1;

    if((heap->left && heap->left->no > heap->no) || (heap->right && heap->right->no > heap->no)) return 0;

    return checkHeap(heap->left) && checkHeap(heap->right);

}

//Checks if tree is heap
int isHeap(HPTree heap){
    if(heap == NULL) return 0;

    int lheight = maxDepth(heap->left);
    int rheight = maxDepth(heap->right);

    if(checkHeap(heap)){
        
        if(lheight < rheight) return 0;
        else if(lheight - rheight > 1) return 0;
        else if(lheight - rheight == 1) return 1;
        else if(lheight - rheight == 0){
            int rsize = heapSize(heap->right);
            int lsize = heapSize(heap->left);

            if(rsize > lsize) return 0;
            else if(rsize == lsize) return (double)lsize == pow(2,(double)lheight) - 1;
            else return 1;
        }
    
    }else return 0;

}

//Makes copy of a node usefull in heapify
HPTree copyNode(HPTree node){

    HPTree temp = malloc(sizeof(heaptree));

    temp->no = node->no;
    temp->Name = malloc(strlen(node->Name)+1);
    strcpy(temp->Name , node->Name);
    temp->parent = node->parent;
    temp->left = node->left;
    temp->right = node->right;

    return temp;
}

//Makes tree heap
HPTree heapify(HPTree root , int i , HPTree *temp){

    HPTree cur = heapLevelOrder(root , &i);

    if(cur->left == NULL) return root;
    else{
        HPTree swap = NULL;
        HPTree help;
        if(cur->right!=NULL){
            if(cur->no >= cur->left->no && cur->no >= cur->right->no){                                  //if nothing to change return
                return root;
            }else if(cur->no < cur->left->no || cur->no < cur->right->no){
                if(cur->left->no >= cur->right->no){
                    if(cur->parent != NULL){
                        if(cur==cur->parent->left){                                                     //if node to change is left child of parent
                            swap = cur->left;
                            help = copyNode(swap);
                            if(swap->left != NULL) swap->left->parent = cur;
                            if(swap->right !=NULL) swap->right->parent = cur;
                            swap->parent = cur->parent;
                            swap->right = cur->right;
                            swap->left = cur;
                            cur->parent->left = swap;
                            cur->parent = swap;
                            cur->left = help->left;
                            cur->right = help->right;
                            swap->right->parent = swap;
                            *temp = swap;
                        }else{                                                                          //if node to change is right child of parent
                            swap = cur->left;
                            help = copyNode(swap);
                            if(swap->left != NULL) swap->left->parent = cur;
                            if(swap->right !=NULL) swap->right->parent = cur;
                            swap->parent = cur->parent;
                            swap->right = cur->right;
                            swap->left = cur;
                            cur->parent->right = swap;
                            cur->parent = swap;
                            cur->left = help->left;
                            cur->right = help->right;
                            swap->right->parent = swap;
                            *temp = swap;
                        }
                        
                        free(help->Name);
                        free(help);
                        return root;

                    }else{                                                                              //if node to change is root
                        swap = cur->left;
                        help = copyNode(swap);
                        if(swap->left != NULL) swap->left->parent = cur;
                        if(swap->right !=NULL) swap->right->parent = cur;
                        swap->parent = cur->parent;
                        swap->right = cur->right;
                        swap->left = cur;
                        cur->parent = swap;
                        cur->left = help->left;
                        cur->right = help->right;
                        swap->right->parent = swap;
                        *temp = swap;

                        free(help->Name);
                        free(help);
                        return swap;
                    }

                }else{                                                                                   //if node to change is right child of parent
                    if(cur->parent!=NULL){
                        if(cur == cur->parent->left){                                                    //Same conditions as earlier
                            swap = cur->right;
                            help = copyNode(swap);
                            if(swap->left != NULL) swap->left->parent = cur;
                            if(swap->right !=NULL) swap->right->parent = cur;
                            swap->parent = cur->parent;
                            swap->left = cur->left;
                            swap->right = cur;
                            cur->parent->left = swap;
                            cur->parent = swap;
                            cur->left = help->left;
                            cur->right = help->right;
                            swap->left->parent = swap;
                            *temp = swap;
                        }else{
                            swap = cur->right;
                            help = copyNode(swap);
                            if(swap->left != NULL) swap->left->parent = cur;
                            if(swap->right !=NULL) swap->right->parent = cur;
                            swap->parent = cur->parent;
                            swap->left = cur->left;
                            swap->right = cur;
                            cur->parent->right = swap;
                            cur->parent = swap;
                            cur->left = help->left;
                            cur->right = help->right;
                            swap->left->parent = swap;
                            *temp = swap;
                        }

                        free(help->Name);
                        free(help);

                        return root;
                    }else{
                        swap = cur->right;
                        help = copyNode(swap);
                        if(swap->left != NULL) swap->left->parent = cur;
                        if(swap->right !=NULL) swap->right->parent = cur;
                        swap->parent = cur->parent;
                        swap->left = cur->left;
                        swap->right = cur;
                        cur->parent = swap;
                        cur->left = help->left;
                        cur->right = help->right;
                        swap->left->parent = swap;
                        *temp = swap;

                        free(help->Name);
                        free(help);

                        return swap;
                    }
                }
            }
        }else{

            if(cur->no >= cur->left->no){
                return root;
            }else{
                if(cur->parent != NULL){
                    if(cur==cur->parent->left){
                        swap = cur->left;
                        help = copyNode(swap);
                        if(swap->left != NULL) swap->left->parent = cur;
                        if(swap->right !=NULL) swap->right->parent = cur;
                        swap->parent = cur->parent;
                        swap->right = cur->right;
                        swap->left = cur;
                        cur->parent->left = swap;
                        cur->parent = swap;
                        cur->left = help->left;
                        cur->right = help->right;
                        *temp = swap;
                    }else{
                        swap = cur->left;
                        help = copyNode(swap);
                        if(swap->left != NULL) swap->left->parent = cur;
                        if(swap->right !=NULL) swap->right->parent = cur;
                        swap->parent = cur->parent;
                        swap->right = cur->right;
                        swap->left = cur;
                        cur->parent->right = swap;
                        cur->parent = swap;
                        cur->left = help->left;
                        cur->right = help->right;
                        *temp = swap;
                    }

                    free(help->Name);
                    free(help);

                    return root;
                }else{
                    swap = cur->left;
                    help = copyNode(swap);
                    if(swap->left != NULL) swap->left->parent = cur;
                    if(swap->right !=NULL) swap->right->parent = cur;
                    swap->parent = cur->parent;
                    swap->right = cur->right;
                    swap->left = cur;
                    cur->parent = swap;
                    cur->left = help->left;
                    cur->right = help->right;
                    *temp = swap;

                    free(help->Name);
                    free(help);

                    return swap;
                }
            }
        }
    }   
}


//Inserts node to heap
HPTree heapInsert(HPTree root , int size , char *virusName , QHead queue){

    if(root==NULL){
        root = malloc(sizeof(heaptree));
        root->no = size;
        root->Name = malloc(strlen(virusName)+1);
        strcpy(root->Name , virusName);
        root->parent = NULL;
        root->left = NULL;
        root->right = NULL;
        enQueue(queue , root);
        return root;
    }

    HPTree temp = getFront(queue);

    if(temp->left == NULL && temp->right == NULL){
        temp->left = heapInsert(temp->left , size , virusName , queue );
        temp->left->parent = temp;
    }else if(temp->left!=NULL && temp->right == NULL){
        temp->right = heapInsert(temp->right , size , virusName , queue);
        temp->right->parent = temp;
    }

    if(temp->left != NULL && temp->right != NULL){
        deQueue(queue);
    }

    return root;

}

//prints heap root and returns new root after heapifing new tree
HPTree heapMax(HPTree root , int size , int counter){

    int y=size;

    //if size = 1 only root left in tree
    if(size == 1){
       // printf("No%d. There have been %d instances of %s\n" , counter , root->no , root->Name);
        printf("%s %d\n", root->Name , root->no);
        free(root->Name);
        free(root);
        return NULL;
    }
    
    HPTree temp;

    //if size = 2 below process is not needed and may leed to errors
    if(size>2){

        temp = heapLevelOrder(root , &y);

        if(temp->parent != NULL){
            if(temp->parent->left == temp){
                temp->parent->left = NULL;
            }else{
                temp->parent->right = NULL;
            }
            temp->parent = NULL;
        }
    }

    printf("No%d. There have been %d instances of %s\n" , counter , root->no , root->Name);
    printf("%s %d\n", root->Name , root->no);
    
    //if only two nodes temp becomes root;
    if(size==2){
        if(root->left!=NULL){
            temp = root->left;
        }else{
            temp = root->right;
        }
        temp->right = NULL;
        temp->left = NULL;
        temp->parent = NULL;
        free(root->Name);
        free(root);
        return temp;
    }

    //swap temp with root
    temp->right = root->right;
    temp->left = root->left;
    if(root->left != NULL) root->left->parent = temp;
    if(root->right != NULL) root->right->parent = temp;
    root->right=NULL;
    root->left=NULL;
    free(root->Name);
    free(root);

    HPTree temp2 = NULL;

    //if tree after temp became root is not a heap reheapify
    if(!isHeap(temp)){
        for(int i=(size-1)/2 ; i >= 1 ; i--){
            temp = heapify(temp , i , &temp2);

        }
    }

    //if tree still not a heap heapify tree with root is temp2
    if(temp2!=NULL && !isHeap(temp)){
        int x = heapSize(temp2);
        HPTree temp3 = NULL;
        while(x > 1){
            for(int i=x/2 ; i > 0 ; i--){
                temp2 = heapify(temp2 , i , &temp3);
            }
            x = heapSize(temp3);
            temp2 = temp3;
            temp3 = NULL;
        }

    }

    return temp;

}

//Free heap
void heapDestroy(HPTree heap){
    if(heap == NULL) return;

    heapDestroy(heap->left);
    heapDestroy(heap->right);

    free(heap->Name);
    free(heap);
}


//////////////////////////////////////////////////////      QUEUE FUNCTIONS        ////////////////////////////////////////////

//Creates Queue
QHead createQueue(){ 
    QHead q = (QHead)malloc(sizeof(Queue)); 
    q->front = q->rear = NULL; 
    return q; 
} 

//Places node in Queue
void enQueue(QHead q, HPTree node){ 
    // Create a new LL node 
    QNode temp = malloc(sizeof(queueNode));
    temp->heapNode = node;
    temp->next = NULL;
  
    // If queue is empty, then new node is front and rear both 
    if (q->rear == NULL) { 
        q->front = q->rear = temp; 
        return; 
    } 
  
    // Add the new node at the end of queue and change rear 
    q->rear->next = temp; 
    q->rear = temp; 
}

//Frees first node in queue
void deQueue(QHead q){ 
    // If queue is empty, return NULL. 
    if (q->front == NULL) 
        return; 
  
    // Store previous front and move front one node ahead 
    QNode temp = q->front; 
  
    q->front = q->front->next; 

    free(temp);
  
    // If front becomes NULL, then change rear also as NULL 
    if (q->front == NULL) 
        q->rear = NULL;

    return;
  
} 

//Returns front node of queue
HPTree getFront(QHead q){

    if(q == NULL) return NULL;
    if(q->front == NULL) return NULL;

    return q->front->heapNode;
}