#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void print_sparse_table(CodeElem table[]) {
    int i;
    for (i=0;i<char_set;i++) {
        if(table[i].length!=0)
            printf("[%c,%d,%d]",table[i].ch,table[i].code,table[i].length);
    }
}

void print_concise_table(CodeElem table[],int table_size) {
    int i;
    for (i=0;i<char_set;i++) {
        if(table[i].length!=0)
            printf("[%c,%d,%d]",table[i].ch,table[i].code,table[i].length);
    }
}

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

int concat_bits(unsigned char *byte,unsigned int bit,int *byte_length) {
    if(*byte_length>=8) {
        return -1; //buffer already full!
    }
    *byte=(*byte<<1)|bit;
    (*byte_length)++;
    if(*byte_length==8) {
        return 1; //buffer now full!
    }
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
int update_freq(int c, FreqElem arr[]) {
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

    return queue;
}

//builds the huffman tree
Node *build_tree(NodeArray *queue) {
    /*printf("initial:\n");
    print_NodeArray(queue);
    printf("\ndequeue results:\n");*/

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

    /*printf("\nqueue is now:\n");
    print_NodeArray(queue);
    printf("\n\ntree:");
    print_tree(parent);*/
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

void write_serialized_tree(FILE *out_file, Node *root,char *buffer, char *buffer_size,unsigned int *count) {
    if(*buffer_size>=8) {
        fwrite(&buffer,sizeof(char),1,out_file);
        *buffer=*buffer_size=0;
        (*count)++;
    }

    if(root->data!=0) {     //here comes a leaf node!
        *buffer=*buffer<<(8-*buffer_size);
        fwrite(buffer,1,1,out_file);
        fwrite(&(root->data),sizeof(char),1,out_file);
        (*count)+=2;
        *buffer=*buffer_size=0;
    }
    else {
        *buffer=(*buffer<<1)|1;
        (*buffer_size)++;
        // (*count)++;
    }

    if(root->left_node!=NULL)
        write_serialized_tree(out_file,root->left_node,buffer,buffer_size,count);
    if(root->right_node!=NULL)
        write_serialized_tree(out_file,root->right_node,buffer,buffer_size,count);
}


void write_encoded_data(FILE *in_file, FILE *out_file,CodeElem *table) {
    rewind(in_file);
    char buffer=0;
    unsigned int code;
    int i,res,buffer_length=0,length=0,c;

    while ((c = fgetc(in_file)) != EOF) {
        code=table[c].code;
        if (table[c].length==0) {   //skipping the char if not in table
            continue;
        }
        length=(int)table[c].length;

        for ( i = length-1; i >= 0; i--) {
            // Extract bits
            unsigned int bit = (code>>i)& 1;
            res=concat_bits(&buffer, bit, &buffer_length);
            if (res==1) {
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

int encode(const char input_file[], const char output_file[]) {
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

    FILE *out_file = fopen(output_file, "wb");
    if (out_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    //1. collect the frequencies
    int c=0;
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

    /*printf("\n\n");
    print_sparse_table(table);
    printf("%d",table_size);*/

    //5. Encoding and writing to output file
    //5.1 serializing and writing the tree
    char buffer=0,buffer_size=0;
    unsigned int serialized_tree_length=0;
    fwrite(&serialized_tree_length,sizeof(unsigned int),1,out_file); //reserve the first 2 bytes for the length
    write_serialized_tree(out_file,tree,&buffer,&buffer_size,&serialized_tree_length);

    rewind(out_file); //move to the front to write the length
    fwrite(&serialized_tree_length,sizeof(unsigned int),1,out_file);
    fseek(out_file,serialized_tree_length+sizeof(unsigned int),SEEK_SET); //move back
    // printf("\n%d,,%d\n",serialized_tree_length,ftell(out_file));

    //5.2 writing the encoded data to the out_file
    write_encoded_data(in_file,out_file,table);

    //freeing all allocated vars.
    free(table);
    free(queue->start);
    free(queue);
    free_tree(tree);
    if(fclose(in_file))
        perror("Failed to close input file!");
    if(fclose(out_file))
        perror("Failed to close output file!");

    printf("\n\nFile successfully outputted to %s",output_file);
    return 0;
}

/*-------------------------------------------decoding--------------------------------------------------*/


//this reads the deserialized tree from file, and returns an array of char
char* read_deserialized_tree(FILE *in_file,int *data_length) {
    fread(data_length,sizeof(unsigned int),1,in_file);
    char *deserialized_tree_array=(char*) malloc(*data_length*sizeof(char));
    if(fread(deserialized_tree_array,sizeof(char),*data_length,in_file)!=*data_length) {
        perror("Failed to read deserialized tree!");
        exit(EXIT_FAILURE);
    }
    return deserialized_tree_array;
}

unsigned int get_bit_at(unsigned int byte, unsigned int pos) {
    return (byte>>(pos-1))&1;
}

Node *new_node() {
    Node *node=(Node*)malloc(sizeof(Node));
    node->data=node->freq=0;
    node->left_node=node->right_node=NULL;
    return node;
}

Node  *deserialize_tree(char *data_array, int *array_index,int *bit_index) {
    Node *node=new_node();
    unsigned int bit=get_bit_at(data_array[*array_index],*bit_index);
    if(bit==0) {
        (*array_index)++;
        node->data=data_array[*array_index];
        (*array_index)++;
        *bit_index=8;
        return node;
    }
    else {
        (*bit_index)--;
        if(*bit_index<=0) {
            *bit_index=8;
            (*array_index)++;
        }
        node->left_node=deserialize_tree(data_array,array_index,bit_index);
        node->right_node=deserialize_tree(data_array,array_index,bit_index);
        return node;
    }
}

//iteratively tranveses tree, and stores the char into c.
//The return value denotes whether it is successful:
// 1: not enough bits; 0: success; -1: empty node.
int find_char(Node *node, const unsigned int *buffer, unsigned int *buffer_length, Node **current_node) {

    while (node->data==0) {
        if(*buffer_length==0) {
            return 1;
        }
        unsigned int dir=get_bit_at(*buffer,*buffer_length);

        switch (dir) {

            case 1:
                if(node->left_node!=NULL) {
                    node=node->left_node; break;
                }
                else
                    return -1; //this node has data 0 (should be internal) but has no children!

            case 0:
                if(node->right_node!=NULL) {
                    node=node->right_node; break;
                }
                else
                    return -1;
        }
        *current_node=node;
        (*buffer_length)--;
    }
    return 0;
}



int read_byte(FILE *in_file,unsigned int *buffer,unsigned int *buffer_length) {
    //since int has a minimum length of 2 bytes, I assume it is 2 bytes.

    if (*buffer_length>8) {
        return 1;
    }
    unsigned char byte;
    if(fread(&byte,1,1,in_file)!=1) {
        return -1; //if unable to read one byte, then EOF reached.
    }
    *buffer=((*buffer)<<8)|byte; //if byte is signed char, when expanding to int, padding on the left is 1 or 0 depending on leading bit...
    //so if the byte starts with 1, padding will be 1, and buffer values are all set to 1...
    *buffer_length+=8;
    return 0;
}


int decode(const char input_file[], const char output_file[]) {

    FILE *in_file=fopen(input_file, "rb");
    if (in_file==NULL) {
        perror("Failed to open input file");
        return 1;
    }

    FILE *out_file=fopen(output_file, "wb"); //wb because inserting \n in Windows corresponds to \n\r, and makes file larger
    if (out_file==NULL) {
        perror("Failed to open output file!");
        fclose(in_file);
        return 1;
    }



    // printf("\n\nDecode:\n");
     int data_length=0, bit_index=8, array_index=0;
    char *data_array=read_deserialized_tree(in_file,&data_length);
    // printf("\nlength is:%d\n",data_length);
    /*for(i=0;i<=data_length+4;i++) {
        printf("%d,",data_array[i]);
    }*/
    // printf("\n");
    Node* tree=deserialize_tree(data_array,&array_index,&bit_index);

    // print_tree(tree);
    // printf("\n\n\nOffset: %d",ftell(in_file));

    char c=0;
    unsigned int buffer=0, buffer_length=0, end_flag=0;
    Node *c_node,*node=tree;

    //reading the file and finding the characters from tree:
    while (1) {
        int read_res=read_byte(in_file,&buffer,&buffer_length);
        int find_res=find_char(node,&buffer,&buffer_length,&c_node);
        switch (find_res) {
            case 0:
                c=c_node->data;
                node=tree;
                fputc(c,out_file);
                break;
            case 1: {
                if (read_res==-1)
                    end_flag=1; //if EOF reached, and buffer is used up, then quit.
                else {
                    node=c_node;
                }
                break;
            }
            case -1:
                printf("Tree error");
        }
        if(end_flag==1)
            break;
    }

    free_tree(tree);
    free(data_array);
    if(fclose(in_file))
        perror("Failed to close input file!");
    if(fclose(out_file))
        perror("Failed to close output file!");
    printf("\n\nFile successfully outputted to %s",output_file);
    return 0;
}

/*-----------------------------main------------------------------------*/


//don't forget to check if malloc gives NULL!
int main(int argc, char *argv[]) {
    const char help[]="Usage: Huffman <option> <input_file> <output_file>\n"
                      "Options:\n"
                      "-e Encode the input file.\n"
                      "-d Decode the input file.\n"
                      "-h Display this help message.\n"
                      "note: if the <output_file> is not specified, the program will default to output.txt / output.huf\n";

    //argc is the number of all segments on terminal, separated by space.
    //So the program call itself is also included.

    //check for minimum number of arguments
    if (argc < 3) {
        printf("Incorrect number of arguments!\n");
        printf("%s",help);
        return 1;
    }

    //defaults
    char *default_output_txt="output.txt";
    char *default_output_huf="output.huf";

    //parse the command-line arguments
    const char *mode = argv[1];
    const char *input_file = argv[2];
    char *output_file = argv[3];


    if (strcmp(mode, "-e") == 0) {
        // Encoding
        if (output_file==NULL)
            output_file=default_output_huf;

        encode(input_file, output_file);

    } else if (strcmp(mode, "-d") == 0) {
        // Decoding
        if (output_file==NULL)
            output_file=default_output_txt;

        decode(input_file, output_file);
    } else {
        // Invalid mode
        fprintf(stderr, "Error: Unknown mode '%s'\n", mode);
        printf("%s",help);
        return 1;
    }

    return 0;

}