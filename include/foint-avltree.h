#ifdef __cplusplus
extern "C" {
#endif
#ifndef _FFAVLTREE_H
#define _FFAVLTREE_H

#include <malloc.h>
#include <stdio.h>
#include "misc.h"   /* for foint */


/*-------------------  Types  ------------------*/

typedef struct _favlTreeNode
{
    foint key, info;
    struct _favlTreeNode *left, *right;
    char balance:2; // 2 bits to store an int between -2 and +1
} FAVLTREENODE;

typedef struct _favlTree
{
    unsigned n; // total number of entries in the tree
    FAVLTREENODE *root;
    pCmpFcn cmpKey;
    pFointCopyFcn copyKey, copyInfo;
    pFointFreeFcn freeKey, freeInfo;
} FAVLTREE;

/*-----------   Function Prototypes  -----------*/

FAVLTREE *FAvlTreeAlloc(pCmpFcn cmpKey, pFointCopyFcn copyKey, pFointFreeFcn freeKey,
    pFointCopyFcn copyInfo, pFointFreeFcn freeInfo);

void FAvlTreeInsert(FAVLTREE *, foint key, foint info); // replaces info if the key already exists

/* O(log n): both lookup and/or delete: returns false if element not found.
** If found and (int)pInfo==1, delete it; otherwise if pInfo != NULL, populate with new info; otherwise just return true.
*/
Boolean FAvlTreeLookDel(FAVLTREE *, foint key, foint *pInfo);
#define FAvlTreeDelete(T,k) FAvlTreeLookDel((T),(k),(foint*)1)
#define FAvlTreeLookup(T,k,f) FAvlTreeLookDel((T),(k), (f))

/*
** FAvlTreeTraverse: Traverse an AVL tree, calling your function pointer (pFointTraversalFcn) on each element,
** in order. It will return -k if k elements were deleted, otherwise 0 or 1 as returned by your function.
*/
int FAvlTreeTraverse (foint globals, FAVLTREE *bt, pFointTraverseFcn);
Boolean FAvlTreeSanityCheck ( FAVLTREE *bt ); // returns true if success, otherwise generates an assertion failure

void FAvlTreeFree(FAVLTREE *);

#endif  /* _FAVLTREE_H */
#ifdef __cplusplus
} // end extern "C"
#endif
