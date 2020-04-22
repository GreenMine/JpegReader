#pragma once

typedef struct AvlTree {
	huffman_code_t huffman;
	struct AvlTree* left;
	struct AvlTree* right;
} avl_tree_t;

typedef struct mainNode {
	struct AvlTree* left;
	struct AvlTree* right;
} main_node_t;

void add_value(main_node_t* node, huffman_code_t huffman) {
	if(node->left == NULL) {
		node->left = create_element(huffman);
	} else {
		node->right = create_element(huffman);
	}
}


avl_tree_t* create_element(huffman_code_t huffman) {
	avl_tree_t* return_value = malloc(sizeof(avl_tree_t));
	return_value->huffman = huffman;
	return return_value;
}