#pragma once

#define COUNT 10

typedef struct HuffmanCode {
	uint8_t length;
	uint8_t count;
	uint16_t code;
	uint8_t value;
} huffman_code_t;

typedef struct AvlTree {
	huffman_code_t huffman;
	struct AvlTree* left;
	struct AvlTree* right;
} avl_tree_t;

//void print2D(avl_tree_t* root, int space)
//{
//	// Base case 
//	if (root == NULL)
//		return;
//
//	// Increase distance between levels 
//	space += COUNT;
//
//	// Process right child first 
//	print2D(root->right, space);
//
//	// Print current node after space 
//	// count 
//	printf("\n");
//	for (int i = COUNT; i < space; i++)
//		printf(" ");
//	if (root->huffman.value == 205)
//		printf("*\n");
//	else
//		printf("%x\n", root->huffman.value);
//
//	// Process left child 
//	print2D(root->left, space);
//}

avl_tree_t* create_element() {
	avl_tree_t* element = malloc(sizeof(avl_tree_t));
	element->left = NULL;
	element->right = NULL;
	return element;
}

avl_tree_t* create_if_not_exists(avl_tree_t** current_node) {
	if(*current_node == NULL)
		*current_node = create_element();
	return *current_node;
}

void add_value(avl_tree_t* node, huffman_code_t huffman) {
	for (int16_t i = log2(huffman.code); i >= 0; i--) 
		node = huffman.code & (1 << i) ? create_if_not_exists(&node->right) : create_if_not_exists(&node->left);
	node->huffman = huffman;
	return;
}

