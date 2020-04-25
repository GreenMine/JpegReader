#pragma once

#define COUNT 10

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

void add_value(avl_tree_t* node, huffman_code_t huffman) {
	avl_tree_t* current = NULL;
	if(node->left == NULL) {
		current = (node->left = create_element());
	} else if(node->right == NULL) {
		current = (node->right = create_element());
	} else {

	}
	if (huffman.length == 1) {
		current->huffman = huffman;
		return;
	}
	huffman.length--;
	add_value(current, huffman);
	return;
}