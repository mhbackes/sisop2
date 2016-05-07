/*
 * bstree.h
 * Created on: 2015-04-19
 *	   Author: mhbackes <mhbackes@inf.ufrgs.br>
 */

#ifndef BSTREE_H
#define BSTREE_H

/* Comparing Function */
typedef int (*CmpFun) (void *, void *);

/* Traversing Function */
typedef void (*TravFun) (void *, void *);

/* Binary-search-tree node struct */
typedef struct bstnode {
	void *key;
	void *data;
	struct bstnode *left;
	struct bstnode *right;
} BSTNode;

/* Binary-search-tree struct */
typedef struct bstree {
	BSTNode *root;
	CmpFun cmp;
	unsigned size;
} BSTree;

/**
 * Creates a tree that uses cmp(n1, n2) as comparing function.
 */
BSTree *bstCreate(CmpFun cmp);

/**
 * Inserts the node into the tree. Returns 0 on success and -1 if key is already
 * in the tree.
 */
int bstInsert(BSTree *tree, void *key, void *data);

/**
 * Returns the data associated with the node that matches the key.
 */
void *bstFind(BSTree *tree, void *key);

/**
 * Removes the node that matches the key. If success, returns the node data,
 * otherwise, returns NULL.
 */
void *bstRemove(BSTree *tree, void *key);

/**
 * Executes fun(node, args) for each node in the tree using in order traversal.
 */
void bstTraverse(BSTree *tree, TravFun fun, void *args);

/**
 * Deletes the tree. 
 * Note: it does not removes the data, only tree nodes!
 */
void bstDelete(BSTree *tree);

#endif /* BSTREE_H */
