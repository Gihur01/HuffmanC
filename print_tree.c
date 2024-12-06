#include <stdio.h>
#include <stdlib.h>
#include "print_tree.h"

// Function to calculate the height of the tree
int tree_height(struct Node *root) {
    if (!root) return 0;
    int left_height = tree_height(root->left_node);
    int right_height = tree_height(root->right_node);
    return (left_height > right_height ? left_height : right_height) + 1;
}

// Function to print all nodes at a given level
void print_level(struct Node *root, int level) {
    if (!root) {
        printf(" ");
        return;
    }
    if (level == 1) {
        printf("[%c/%d,%d] ", root->data, root->data, root->freq);
    } else if (level > 1) {
        print_level(root->left_node, level - 1);
        print_level(root->right_node, level - 1);
    }
}

// Main function to print the tree layer by layer
void print_tree(struct Node *root) {
    int height = tree_height(root);
    int i;
    for ( i = 1; i <= height; i++) {
        print_level(root, i);
        printf("\n");
    }
}

// Utility function to create a new node
struct Node *create_node(int data) {
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->data = data;
    new_node->left_node = NULL;
    new_node->right_node = NULL;
    return new_node;
}