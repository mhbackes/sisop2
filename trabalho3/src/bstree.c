/*
 * bstree.c
 * Created on: 2016-04-19
 *     Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#include "bstree.h"

#include <stdlib.h>

BSTNode *bstCreateNode(void *key, void *data);
BSTNode *bstRemoveNode(BSTNode *x, void *key, void **data, CmpFun cmp);
BSTNode *bstMin(BSTNode *x);
BSTNode *bstRemoveMin(BSTNode *x);
void bstTraverseNode(BSTNode *x, TravFun fun, void *args);
void bstDeleteNode(BSTNode *x);


BSTree *bstCreate(CmpFun cmp) {
	BSTree *tree = malloc(sizeof(BSTree));
	tree->root = NULL;
	tree->cmp = cmp;
	tree->size = 0;
	return tree;
}

int bstInsert(BSTree *tree, void *key, void *data) {
	if(!tree->root) {
		tree->root = bstCreateNode(key, data);
		tree->size++;
	   	return 0;
	}
	BSTNode *parent = NULL;
	BSTNode *x = tree->root;
	while (x) {
		parent = x;
		int cmp = tree->cmp(key, x->key);
		if (cmp < 0) x = x->left;
		else if (cmp > 0) x = x->right;
		else return -1;
	}
	int cmp = tree->cmp(key, parent->key);
	BSTNode *new = bstCreateNode(key, data);
	if (cmp < 0) parent->left = new;
	else parent->right = new;	
	tree->size++;
	return 0;
}

void *bstFind(BSTree *tree, void *key) {
	BSTNode *x = tree->root;
	while (x != NULL) {
		int cmp = tree->cmp(key, x->key);
		if (cmp < 0) x = x->left;
		else if (cmp > 0) x = x->right;
		else return x->data;
	}
	return NULL;
}

void *bstRemove(BSTree *tree, void *key) {
	void *data = NULL;
	tree->root = bstRemoveNode(tree->root, key, &data, tree->cmp);
	if(data) tree->size--;
	return data;
}

BSTNode *bstRemoveMin(BSTNode *x) {
	if(!x->left) return x->right;
	x->left = bstRemoveMin(x->left);
	return x;
}

BSTNode *bstMin(BSTNode *x) {
	if (!x->left) return x;
	return bstMin(x->left);
}

BSTNode *bstRemoveNode(BSTNode *x, void *key, void **data, CmpFun cmp) {
	if (!x) return NULL;
	int c = cmp(key, x->key); 
	if (c < 0) x->left = bstRemoveNode(x->left, key, data, cmp);
	else if (c > 0) x->right = bstRemoveNode(x->right, key, data, cmp);
	else {
		*data = x->data;
		if (!x->right) return x->left;
		if (!x->left) return x->right;
		BSTNode *t = x;
		x = bstMin(t->right);
		x->right = bstRemoveMin(t->right);
		x->left = t->left;
		free(t);
	}
	return x;
}

void bstTraverse(BSTree *tree, TravFun fun, void *args) {
	bstTraverseNode(tree->root, fun, args);
}

void bstTraverseNode(BSTNode *x, TravFun fun, void *args) {
	if(!x) return;
	bstTraverseNode(x->left, fun, args);
	fun(x->data, args);
	bstTraverseNode(x->right, fun, args);
}

void bstDelete(BSTree *tree) {
	bstDeleteNode(tree->root);
	free(tree);
}

void bstDeleteNode(BSTNode *x) {
	if(!x) return;
	bstDeleteNode(x->left);
	bstDeleteNode(x->right);
	free(x);
}

BSTNode *bstCreateNode(void *key, void *data) {
	BSTNode *node = malloc(sizeof(BSTNode));
	node->key = key;
	node->data = data;
	node->left = node->right = NULL;
	return node;
}
