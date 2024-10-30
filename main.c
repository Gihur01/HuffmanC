#include <stdio.h>
struct Node {
    struct Node *left_node;
    struct Node *right_node;
    char left_data;
    char right_data;
};

typedef struct Node Node;

//array of nodes with length. Points to the first node.
typedef struct {
    char data;
    int freq;
} FreqElem;

typedef struct {
    FreqElem *first_elem;
    int length;
} FreqArray;

//insert a char into left or right of node:
//depending on the value of pos
void insert_char(Node *node,char c,int pos) {
    switch (pos) {
        case 0: node->left_data=c; break;
        case 1: node->right_data=c; break;
    }
}

//add a node below another
void insert_node(Node *parent, Node *child, int pos) {
    switch (pos) {
        case 0: parent->left_node=child; break;
        case 1: parent->right_node=child; break;
    }

}

//count the freq (ASCII chars)
FreqArray *count_freq_ASCII(char text[]) {
    FreqElem arr[128];
    int i=0;



}
//builds the tree: input an array of nodes,
//and array of frequencies.
Node *build_tree(FreqArray arr);

//encoding function: opens file, collects freq, then build tree.
//Go through the file again and write the bin code from tree into new file
//then write out encoded, and the key.

void encode(char input_file[], char output_file[]);

void decode(char input_file[], char output_file[]);


//don't forget to check if malloc gives NULL!
int main() {

    return 0;
}