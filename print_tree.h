//
// Created by Frank on 11/23/2024.
//

#ifndef PRINT_TREE_H
#define PRINT_TREE_H

struct Node {
    struct Node *left_node;
    struct Node *right_node;
    char data;
    int freq;
};

// Function prototypes
int tree_height(struct Node *root);
void print_level(struct Node *root, int level);
void print_tree(struct Node *root);
struct Node *create_node(int data);

#endif //PRINT_TREE_H
