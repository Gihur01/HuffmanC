#include <stdio.h>
#include <stdlib.h>

const int char_set=128;
//This global var sets the size of the usable character set. (Currently ASCII)
//Can be later changed to include a larger one

struct Node {
    struct Node *left_node;
    struct Node *right_node;
    char data;
    char freq;
};

typedef struct Node Node;

typedef struct {
    Node *start;
    int len;
    int last;
} NodeArray;

typedef struct {
    char data;
    int freq;
} FreqElem;

//array of nodes with length. Points to the first node.

/*typedef struct {
    FreqElem *first_elem;
    int length;
} FreqArray;*/

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


    if(last>0) {
        //swap with parent
        while (1) {
            //This is a bit confusing but should work :)
            Node *parent=arr->start+(index-1)/2;
            if(node->freq>=parent->freq) {
                break;
            }
            Node *temp=node;
            *node=*parent;
            *parent=*temp;
            index=(index-1)/2;
    }
    }

    arr->start[index]=*node;

}

void dequeue(Node *arr);

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
    FreqElem *queue;
    int i;
    int size=0;
    for (i=0;i<char_set;i++) {
        if (arr[i].freq!=0)
            size++;
    }

    queue=(FreqElem*)malloc(size*sizeof(FreqElem));


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

    //for testing purposes, output values in freq array
    //to the output file
    for(i=0;i<128;i++) {
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