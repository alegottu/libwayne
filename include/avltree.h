#ifdef __cplusplus
extern "C" {
#endif
#ifndef _AVLTREE_H
#define _AVLTREE_H

#include <malloc.h>
#include <stdio.h>
#include "misc.h"   /* for foint */


/*-------------------  Types  ------------------*/

typedef struct _avlTreeNode
{
    foint key, info;
    struct _avlTreeNode *left, *right;
    char balance:2; // 2 bits to store an int between -2 and +1
} AVLTREENODE;

typedef struct _avlTree
{
    unsigned n; // total number of entries in the tree
    AVLTREENODE *root;
    pCmpFcn cmpKey;
    pFointCopyFcn copyKey, copyInfo;
    pFointFreeFcn freeKey, freeInfo;
} AVLTREE;

/*-----------   Function Prototypes  -----------*/

AVLTREE *AvlTreeAlloc(pCmpFcn cmpKey, pFointCopyFcn copyKey, pFointFreeFcn freeKey,
    pFointCopyFcn copyInfo, pFointFreeFcn freeInfo);

void AvlTreeInsert(AVLTREE *, foint key, foint info); // replaces info if the key already exists

// returns a foint* so that you can modify the element without having to re-insert it; 
// returns NULL upon failure; deletes the element when delete is true
foint* const AvlTreeLookDel(AVLTREE *, foint key, Boolean delete);
// "safe" version, just returns a value (if requested), checks pointer for you
const foint SAvlTreeLookDel(AVLTREE *, foint key, Boolean delete);
#define AvlTreeDelete(T,k) SAvlTreeLookDel((T),(k),true)
#define AvlTreeLookup(T,k) AvlTreeLookDel((T),(k),false)
#define SAvlTreeLookup(T,k) SAvlTreeLookDel((T),(k),false)

/*
** AvlTreeTraverse: Traverse an AVL tree, calling your function pointer (pFointTraversalFcn) on each element,
** in order. It will return -k if k elements were deleted, otherwise 0 or 1 as returned by your function.
*/
int AvlTreeTraverse (foint globals, AVLTREE *bt, pFointTraverseFcn);
Boolean AvlTreeSanityCheck ( AVLTREE *bt ); // returns true if success, otherwise generates an assertion failure

void AvlTreeFree(AVLTREE *);

#endif  /* _AVLTREE_H */
#ifdef __cplusplus
} // end extern "C"
#endif
