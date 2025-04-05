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
    awk_value_t key, info;
    struct _avlTreeNode *left, *right;
    char balance:2; // 2 bits to store an int between -2 and +1
} AVLTREENODE;

typedef struct _avlTree
{
    unsigned n; // total number of entries in the tree
    AVLTREENODE *root;
    pTreeCmpFcn cmpKey;
    pTreeCopyFcn copyKey, copyInfo;
    pTreeFreeFcn freeKey, freeInfo;
} AVLTREE;

/*-----------   Function Prototypes  -----------*/

AVLTREE *AvlTreeAlloc(pTreeCmpFcn cmpKey, pTreeCopyFcn copyKey, pTreeFreeFcn freeKey,
    pTreeCopyFcn copyInfo, pTreeFreeFcn freeInfo);

void AvlTreeInsert(AVLTREE *, awk_value_t key, awk_value_t info); // replaces info if the key already exists

/* O(log n): both lookup and/or delete: returns false if element not found.
** If found and (int)pInfo==1, delete it; otherwise if pInfo != NULL, populate with new info; otherwise just return true.
*/
Boolean AvlTreeLookDel(AVLTREE *, awk_value_t key, awk_value_t *pInfo);
#define AvlTreeDelete(T,k) AvlTreeLookDel((T),(k),(awk_value_t*)1)
#define AvlTreeLookup(T,k,f) AvlTreeLookDel((T),(k), (f))

/*
** AvlTreeTraverse: Traverse an AVL tree, calling your function pointer (pFointTraversalFcn) on each element,
** in order. It will return -k if k elements were deleted, otherwise 0 or 1 as returned by your function.
*/
int AvlTreeTraverse (foint globals, AVLTREE *bt, pTreeTraverseFcn);
Boolean AvlTreeSanityCheck ( AVLTREE *bt ); // returns true if success, otherwise generates an assertion failure

void AvlTreeFree(AVLTREE *);

#endif  /* _AVLTREE_H */
#ifdef __cplusplus
} // end extern "C"
#endif
