#include <stdio.h>
#include <stdlib.h>

const int char_set=128;
//This global var sets the size of the usable character set. (Currently ASCII)
//Can be later changed to include a larger one

struct Node {
    struct Node *left_node;
    struct Node *right_node;
    char data;
    int freq;
};

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




//insert a char into left or right of node:
//depending on the value of pos
// void insert_char(Node *node,char c,int pos) {
//     switch (pos) {
//         case 0: node->left_data=c; break;
//         case 1: node->right_data=c; break;
//     }
// }

//add a node below another
void insert_node(Node *parent, Node *child, int pos) {
    switch (pos) {
        case 0: parent->left_node=child; break;
        case 1: parent->right_node=child; break;
    }

}

void print_Node(Node node) {
    printf("[%c,%d],",node.data,node.freq);
}

void print_NodeArray(NodeArray *arr) {
    int i;
    int last=arr->last;
    for (i=0;i<=last-1;i++) {
        printf("[%c,%d],",arr->start[i].data,arr->start[i].freq);
        // fprintf(log_file,"[%c,%d],",arr->start[i].data,arr->start[i].freq);
    }
    // fprintf(log_file,"    ");

}

/*To create a Huffman tree from a freq list, a priority queue is used.
This queue is implemented by a min-heap, where the smallest node is at the beginning.*/
//to enqueue, add new node at end, and swap with parent(s) until it is smaller than its parent.
void enqueue(NodeArray *arr,Node *node,FILE *log_file) {
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
Node dequeue(NodeArray *arr) {
    Node nullNode={NULL,NULL,0,0};
    int last=arr->last;
    if(last<=0) {
        printf("The queue is empty!");
        return nullNode;
    }

    Node to_return=arr->start[0]; //so that it isn't lost
    arr->start[0]=arr->start[last-1]; //last-1, bcs last points to next available space
    arr->start[last-1] = nullNode;

    int index=0;
    Node *parent, *child, temp;

    arr->last--;
    while(2*index+1 < last) {
        parent=arr->start+index;
        child=arr->start+2*index+1;

        if(parent->freq < child->freq)
            break;
        temp=*parent;
        *parent=*child;
        *child=temp;

        index=index*2+1;
    }

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



Node *build_tree(FreqElem *arr) {
    FILE *log_file=fopen("log.txt","w");

    Node *queue;
    int i;
    int size=0;
    Node node;
    queue=(Node*)malloc(char_set*sizeof(Node));
    NodeArray node_array={queue,char_set,0};
    for (i=0;i<char_set;i++) {
        if (arr[i].freq!=0) {
            node.left_node=node.right_node=NULL;
            node.data=arr[i].data;
            node.freq=arr[i].freq;
            enqueue(&node_array,&node,log_file);
        }

    }

    //test print
    /*print_NodeArray(&node_array);
    printf("\n\n");
    Node temp_node=dequeue(&node_array);
    print_NodeArray(&node_array);
    printf("\nNode is: ");
    print_Node(temp_node);*/

    for (i=0;i<char_set;i++) {

    }

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

    build_tree(freq_arr);

    //for testing purposes, output values in freq array
    //to the output file
    for(i=0;i<127;i++) {
        fprintf(out_file,",[%c,%d]",freq_arr[i].data,freq_arr[i].freq);
        if (i%5==0)
            fprintf(out_file,"\n");
    }


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