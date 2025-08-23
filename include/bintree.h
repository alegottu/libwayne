#ifdef __cplusplus
extern "C" {
#endif
#ifndef _BINTREE_H
#define _BINTREE_H

#include <malloc.h>
#include <stdio.h>
#include "misc.h"   /* for foint */


/*-------------------  Types  ------------------*/

typedef struct _binTreeNode
{
    foint key, info;
    struct _binTreeNode *left, *right;
    Boolean deleted;
} BINTREENODE;

typedef struct _binTree
{
    unsigned n, physical_n, maxDepth, depthSum, depthSamples; // number of entries & tree depth stats which can be averaged anytime.
    BINTREENODE *root;
    pCmpFcn cmpKey;
    pFointCopyFcn copyKey, copyInfo;
    pFointFreeFcn freeKey, freeInfo;
} BINTREE;

/*-----------   Function Prototypes  -----------*/

BINTREE *BinTreeAlloc(pCmpFcn cmpKey, pFointCopyFcn copyKey, pFointFreeFcn freeKey,
	    pFointCopyFcn copyInfo, pFointFreeFcn freeInfo);

// replaces info if the key already exists, returns a pointer to the info inserted
foint* const BinTreeInsert(BINTREE *, foint key, foint info);

// returns a foint* so that you can modify the element without having to re-insert it; 
// returns NULL upon failure; deletes the element when delete is true
foint* const BinTreeLookup(BINTREE *, foint key);
const Boolean BinTreeDelete(BINTREE *, foint key);
// "safe" version, just gives a value, returns false if not found
const Boolean SBinTreeLookup(BINTREE *, foint key, foint* pInfo);
void BinTreeDelNode(BINTREE *tree, BINTREENODE *p, BINTREENODE **P); // O(1): delete exactly one node

/*
** BinTreeTraverse: Traverse a binary tree, calling your function pointer (pFointTraversalFcn) on each
** element, in order. Your tree should return 1 to continue, 0 to stop, and -1 to DELETE the current node.
** BinTreeTraverse will return -k if k elements were deleted, otherwise 0 or 1 as returned by your functions.
*/
int BinTreeTraverse (foint globals, BINTREE *bt, pFointTraverseFcn);
Boolean BinTreeSanityCheck ( BINTREE *bt ); // returns true if success, otherwise generates an assertion failure
void BinTreeRebalance(BINTREE *tree, Boolean force);

void BinTreeFree(BINTREE *);

#endif  /* _BINTREE_H */
#ifdef __cplusplus
} // end extern "C"
#endif
