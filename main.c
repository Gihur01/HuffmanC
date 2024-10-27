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
    Node *start_node;
    int length;
} NodeArray;

//insert a char into left or right of node:
//depending on the value of pos
Node *insert_char(Node node,char c,int pos);

//add a node below another
//returns the pointer to the parent node
Node *insert_node(Node parent, Node child, int pos);

//builds the tree: input an array of nodes,
//and array of frequencies.
Node *build_tree(NodeArray node_arr, int freq_arr[]);

//encoding function: opens file, collects freq, then build tree.
//Go through the file again and write the bin code from tree into new file
//then write out encoded, and the key.
//TODO: think about how to store key file with the encoded file.
void encode(Node parent_node);




int main() {


}