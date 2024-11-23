#include <stdio.h>
#include <stdlib.h>
#include "print_tree.h"

const int char_set=128;
//This global var sets the size of the usable character set. (Currently ASCII)
//Can be later changed to include a larger one

/*struct Node {
    struct Node *left_node;
    struct Node *right_node;
    char data;
    int freq;
};*/

typedef struct Node Node;

//array of nodes with length. Points to the first node.
typedef struct {
    Node *start;
    int len;
    int last; //points to next available space
} NodeArray;

typedef struct {
    char data;
    int freq;
} FreqElem;


//add a node below another
void insert_node_at(Node *parent, Node *child, int pos) {
    switch (pos) {
        case 0: parent->left_node=child; break;
        case 1: parent->right_node=child; break;
    }
}

void insert_node(Node *parent,Node *child) {
    if(parent->left_node==NULL)
        parent->left_node=child;
    else if(parent->right_node==NULL)
        parent->right_node=child;
    else
        printf("No space for child!");
}


void print_Node(Node node) {
    printf("[%c,%d],",node.data,node.freq);
}

void print_NodeArray(NodeArray *arr) {
    int i;
    int last=arr->last;
    printf("last:%d, ",arr->last);
    for (i=0;i<15;i++) {
        printf("[%c,%d],",arr->start[i].data,arr->start[i].freq);

        // fprintf(log_file,"[%c,%d],",arr->start[i].data,arr->start[i].freq);
    }
    // fprintf(log_file,"    ");
}

/*void print_tree(Node *node) {
    printf(",%c",node->data);
    if(node->left_node!=NULL){
        printf(" left: ");
        print_tree(node->left_node);
    }
    if (node->right_node!=NULL){
        printf(" right: ");
        print_tree(node->right_node);
    }
}*/

void free_tree(Node *node) {
    if(node->left_node!=NULL) {
        free_tree(node->left_node);
    }
    if(node->right_node!=NULL) {
        free_tree(node->right_node);
    }
    else
        free(node);

}

/*To create a Huffman tree from a freq list, a priority queue is used.
This queue is implemented by a min-heap, where the smallest node is at the beginning.*/
//to enqueue, add new node at end, and swap with parent(s) until it is smaller than its parent.
void enqueue(NodeArray *arr,Node *node) {
    int last=arr->last;
    int len=arr->len;
    int index=last; //the index of the new node
    if (last>=len) {
        perror("Error when enqueuing: last exceeding length");
        return;
    }

    arr->start[index]=*node;
    if(last>0) {
        Node temp;
        Node *child=arr->start+index; //new pointer to the new node
        //swap with parent
        while (1) {
            Node *parent=arr->start+(index-1)/2; //In this queue, the parent is at (index-1)/2
            if(child->freq >= parent->freq) {
                break;
            }
            temp=*child;
            *child=*parent;
            *parent=temp;
            index=(index-1)/2;
            child=parent;
        }
    }
    arr->last++;
}

//swap the root with the lowermost branch, and remove it.
//Then keep on swapping the new root with its child until heap property satisfied.
//returns a null node if empty!
Node dequeue(NodeArray *arr) {
    Node nullNode={NULL,NULL,0,0};
    int last=arr->last;
    if(last<=0) {
        printf("The queue is empty! last is: %d\n",arr->last);
        return nullNode;
    }

    Node to_return=arr->start[0]; //so that it isn't lost
    arr->start[0]=arr->start[last-1]; //last-1, bcs last points to next available space
    arr->start[last-1] = nullNode;

    int index=0;
    Node *parent, *child, *child2, temp; //child2 is the right child. needed to compare which child is smaller.

    arr->last--;
    last--;
    while(2*index+1 < last) {
        parent=arr->start+index;
        child=arr->start+2*index+1;
        child2=arr->start+2*index+2;
        if(child2->freq < child->freq && child2->freq>0) {
            child=child2;
        }

        if(parent->freq < child->freq)
            break;
        temp=*parent;
        *parent=*child;
        *child=temp;

        index=index*2+1;
    }
    print_Node(to_return);
    printf("\n");
    print_NodeArray(arr);
    printf("\n");

    return to_return;

}


//takes one char and the frequency list,
//and updates the corresponding frequency
//IMPORTANT: c is counted at the index of its ascii code
int update_freq(char c, FreqElem arr[]) {
    if (c>=char_set) {
        //PANIC!!
        perror("Char is larger than max character value??");
        return -1;
    }
    arr[c].freq+=1;
    return 0;
}


//This function builds the priority queue, from which the tree is constructed.

NodeArray *build_queue(FreqElem *arr) {
    // FILE *log_file=fopen("log.txt","w");

    Node *node_list;
    int i;
    int size=0;
    Node node;
    node_list=(Node*)malloc(char_set*sizeof(Node));

    NodeArray *queue=(NodeArray*)malloc(sizeof(NodeArray));
    queue->start=node_list;
    queue->len=char_set;
    queue->last=0;

    for (i=0;i<char_set;i++) {
        if (arr[i].freq!=0) {
            node.left_node=node.right_node=NULL;
            node.data=arr[i].data;
            node.freq=arr[i].freq;
            enqueue(queue,&node);
        }

    }

    //test print
    /*print_NodeArray(&node_array);
    printf("\n\n");
    Node temp_node=dequeue(&node_array);
    print_NodeArray(&node_array);
    printf("\nNode is: ");
    print_Node(temp_node);*/

    return queue;
}

//builds the huffman tree
Node *build_tree(NodeArray *queue) {
    printf("initial");
    print_NodeArray(queue);
    printf("\ndequeue results:\n");
    Node nullNode={NULL,NULL,0,0};
    if(queue->last<=0)
        return NULL;
    Node *parent; //a new node, parent of the 2 smallest ones.
    Node *child1,*child2; //2 smallest nodes
    while (queue->last>1) {
        parent=(Node*)malloc(sizeof(Node));
        parent->left_node=parent->right_node=NULL;
        parent->data=0;
        child1=(Node*)malloc(sizeof(Node));
        child2=(Node*)malloc(sizeof(Node));
        *child1=dequeue(queue);
        *child2=dequeue(queue);
        printf("\n");
        insert_node_at(parent,child1,0);
        insert_node_at(parent,child2,1);
        parent->freq=child1->freq+child2->freq;
        enqueue(queue,parent);

    }
    printf("\nqueue is now:\n");
    print_NodeArray(queue);
    printf("\n\ntree:");
    print_tree(parent);

    return parent;

}


//encoding function: opens file, collects freq, then build tree.
//Go through the file again and write the bin code from tree into new file
//then write out encoded, and the key.

int encode(char input_file[], char output_file[]) {
    int i,j;

    //init the frequency array
    FreqElem freq_arr[128];
    for (i=0;i<128;i++) {
        freq_arr[i].data=i; //auto convert to ascii char
        freq_arr[i].freq=0;
    }

    //opening both files
    FILE *in_file = fopen(input_file, "r");
    if (in_file == NULL) {
        perror("Error opening input file");
        return 1;
    }

    FILE *out_file = fopen(output_file, "w");
    if (out_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    //update frequencies
    char c;
    while ((c = fgetc(in_file)) != EOF) {  // Read character by character until end of file
        update_freq(c,freq_arr);
    }

    NodeArray *queue = build_queue(freq_arr);
    Node *tree=build_tree(queue);



    //for testing purposes, output values in freq array
    //to the output file
    for(i=0;i<127;i++) {
        fprintf(out_file,",[%c,%d]",freq_arr[i].data,freq_arr[i].freq);
        if (i%5==0)
            fprintf(out_file,"\n");
    }

    free(queue);
    free_tree(tree);
    fclose(in_file);
    fclose(out_file);
    return 0;
}

void decode(char input_file[], char output_file[]);


//don't forget to check if malloc gives NULL!
int main() {
    encode("input.txt","output.txt");
    return 0;
}