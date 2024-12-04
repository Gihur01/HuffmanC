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

typedef struct {
    char ch;
    unsigned int code;
    char length;
} CodeElem;


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
    }
}

void print_table(CodeElem table[]) {
    int i;
    for (i=0;i<char_set;i++) {
        if(table[i].length!=0)
            printf("[%c,%d,%d]",table[i].ch,table[i].code,table[i].length);
    }
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

int concat_bits(char *byte,char bit,int *byte_length) {
    if(*byte_length>7) {
        return 1;
    }
    *byte=(*byte<<1)|bit;
    (*byte_length)++;
    return 0;
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
    /*print_Node(to_return);
    printf("\n");
    print_NodeArray(arr);
    printf("\n");*/

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
    /*
     * in each iteration, 2 nodes are removed from the priority queue. (the smallest 2)
     * a new Node, parent, is created, which points to each of them. the frequency is the sum of children.
     * parent is inserted back into the queue.
     * repeat above until only 1 node remains.
     */
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
        // printf("\n");
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

void build_table(CodeElem *table,Node *node,int code, int length) {
    length++;
    if(node->left_node!=NULL) {
        build_table(table,node->left_node,(code << 1)| 1,length);
    }
    if(node->right_node!=NULL) {
        build_table(table,node->right_node,(code<<1),length);
    }
    else {
        length--;
        char i=node->data;
        table[i].code=code;
        table[i].length=length;
        table[i].ch=i;
    }
}

//This recursive function traverses the tree, and writes the serialized values into the data_array. (pre-order traverse)
/* every * node is 1; every edge node is 0+ascii code
 *e.g. tree:
 *      *
 *     A *
 *      B C
 *the result should look like: 10A10B0C (but in binary)
 *Since it is hard to traverse tree and write to file at the same time, I decided to write all the parts into an array, and concat later.*/
//since the chars are all ascii, the data_array size is char. change to larger type if working with Unicode.
int serialize_tree(Node *root,char *data_array, int array_index) {
    if (root->data==0)
        data_array[array_index]=1;
    else {
        data_array[array_index]=0;
        data_array[++array_index]=root->data;
        return array_index;
    }
    if(root->left_node!=NULL) {
        array_index=serialize_tree(root->left_node,data_array,array_index+1);
    }
    if(root->right_node!=NULL) {
        array_index=serialize_tree(root->right_node,data_array,array_index+1);
    }

    return array_index; //final length of the data array
}

/*void write_serialized_tree(FILE *file, Node *root,char buffer, char buffer_size) {
    if(buffer_size>=8) {
        fwrite(&buffer,sizeof(char),1,file);
        buffer=buffer_size=0;
    }

    if(root->freq!=0) {     //here comes a leaf node!
        int shift=8-buffer_size;
        //since a char is 1 byte, the buffer is 100% full, so here I fill the buffer with part of the char.
        buffer=(buffer<<(shift));
        buffer=buffer|((root->data)>>buffer_size);
        fwrite(&buffer,sizeof(char),1,file);
        buffer=((root->data)<<shift)>>shift; //setting buffer to remaining part of the char!
        buffer_size=shift;
        return;
    }

    buffer=(buffer<<1)|1;
    buffer_size++;
    if(root->left_node!=NULL);
}*/

void write_code_table(FILE *file, CodeElem *table, int table_size) {
    int i;

    if (file == NULL) {
        perror("No file!");
        exit(EXIT_FAILURE);
    }

    // Writing the number of entries in table
    if (fwrite(&table_size, sizeof(unsigned int), 1, file) != 1) {
        perror("Failed to write table size");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Writing each entry in the table:
    for (i = 0; i < char_set; i++) {
        if(table[i].length<=0) {
            continue;   //Quickly skipping each empty entry...
        }

        // Writing the character
        if (fwrite(&table[i].ch, sizeof(char), 1, file) != 1) {
            perror("Failed to write character");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Writing the code
        if (fwrite(&table[i].code, sizeof(unsigned int), 1, file) != 1) {
            perror("Failed to write code");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Writing the length of the code
        if (fwrite(&table[i].length, sizeof(char), 1, file) != 1) {
            perror("Failed to write length");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

}

void write_encoded_data(FILE *in_file, FILE *out_file,CodeElem *table) {
    rewind(in_file);
    char buffer=0,c;
    unsigned int code;
    int i,res,buffer_length=0;
    while ((c = fgetc(in_file)) != EOF) {
        code=table[c].code;
        if (table[c].length==0) {
            continue;
        }
        for ( i = table[c].length - 1; i >= 0; i--) {
            // Extract bits
            char bit = (code >> i) & 1;
            res=concat_bits(&buffer, bit, &buffer_length);
            if (res) {
                fwrite(&buffer, 1, 1, out_file); // Write the byte to the file
                buffer = 0;                      // Reset the buffer
                buffer_length = 0;

            }
        }
    }
    if (buffer_length > 0) {
        buffer = buffer << (8 - buffer_length); // Pad remaining bits with 0s
        fwrite(&buffer, 1, 1, out_file);
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

    //1. collect the frequencies
    char c;
    while ((c = fgetc(in_file)) != EOF) {  // Read character by character until end of file
        update_freq(c,freq_arr);
    }

    //2. construct the priority queue
    NodeArray *queue = build_queue(freq_arr);

    //3. construct the huffman tree
    Node *tree=build_tree(queue);

    //4. construct the encoding table from the tree
    //4.1 initializing the table (the build_table function is recursive)
    CodeElem *table=(CodeElem*)malloc(char_set*sizeof(CodeElem));
    for (i=0;i<char_set;i++) {
        table[i].code=table[i].length=table[i].ch=0;
    }
    build_table(table,tree,0,0);

    //4.2 getting the table size
    int table_size=0;
    for (i=0;i<char_set;i++) {
        if(table[i].length!=0)
            table_size++;
    }

    //print_table(table);

    //5. Encoding and writing to output file
    //5.1 serializing the tree and writing to file:
    char *serial_tree_array=(char*) malloc(sizeof(char)*table_size*4); //data array that holds the serialized tree; 4x the length of the table, just in case.
    int array_size=serialize_tree(tree,serial_tree_array,0);

    char temp_byte=0;
    int temp_byte_length=0;
    for(i=0;i<table_size*4;i++) { //loop through each data element...
        char current_data=serial_tree_array[i];
        printf("%c,",current_data>=32 ? current_data : current_data+'0');
        if(current_data!=1 && current_data!=0) { //if the data element is a char, then...
            for(j=7;j>=0;j--) { //go through all bits in the char
                if(concat_bits(&temp_byte,current_data & (1<<j),&temp_byte_length)==1) { //if the buffer is filled halfway, write to file, and empty it
                    fwrite(&temp_byte,1,1,out_file);
                    temp_byte=0;
                    temp_byte_length=0;
                }
            }
        }
        else {
            if(concat_bits(&temp_byte,current_data,&temp_byte_length)==1) {
                fwrite(&temp_byte,1,1,out_file);
                temp_byte=0;
                temp_byte_length=0;
            }
        }


    }

    // write_code_table(out_file,table,table_size);

    //5.2 writing the encoded data to the out_file
    // write_encoded_data(in_file,out_file,table);

    //freeing all allocated vars.
    free(table);
    free(queue);
    free_tree(tree);
    fclose(in_file);
    fclose(out_file);
    return 0;
}

CodeElem *read_encoding_table(FILE *file,unsigned int *tableSize) {
    int i,res;
    char c;
    if (fread(tableSize,sizeof(unsigned int),1,file) != 1) {
        perror("Failed to read table size");
        exit(EXIT_FAILURE);
    }

    CodeElem *table = (CodeElem *)malloc(*tableSize*sizeof(CodeElem));
    if (table == NULL) {
        perror("Memory allocation failed for table");
        exit(EXIT_FAILURE);
    }

    for ( i = 0; i < *tableSize; i++) {
        if (fread(&table[i].ch,sizeof(char),1,file) !=1 ||
            fread(&table[i].code,sizeof(unsigned int), 1,file) != 1 ||
            fread(&table[i].length,sizeof(char),1,file)!= 1) {

            perror("Failed to read encoding table entry");
            free(table);
            exit(EXIT_FAILURE);
            }
    }

    printf("\nRead table is:\n");
    print_table(table);
    return table;
}

void decode(char input_file[], char output_file[]) {
    int i;

    FILE *in_file=fopen(input_file, "rb");
    if (in_file==NULL) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    FILE *outFile=fopen(output_file, "w");
    if (outFile==NULL) {
        perror("Failed to open output file");
        fclose(in_file);
        exit(EXIT_FAILURE);
    }

    // Read the encoding table
    unsigned int tableSize;
    CodeElem *table = read_encoding_table(in_file, &tableSize);

    char byte;
    char buffer=0;
    int buffer_size = 0;

    while (fread(&byte, 1, 1, in_file) == 1) {
        buffer=buffer<<1;
        buffer_size++;
        buffer=buffer|(byte>>7);
        byte=byte<<1;




    }

    free(table);
}

//don't forget to check if malloc gives NULL!
int main() {
    encode("input.txt","output.txt");
    return 0;
}