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
	char* key;
    awk_value_t info;
    struct _avlTreeNode *left, *right;
    char balance:2; // 2 bits to store an int between -2 and +1
} AVLTREENODE;

// TODO: can potentially make copyInfo & freeInfo predef as well
typedef struct _avlTree
{
    unsigned n; // total number of entries in the tree
    AVLTREENODE *root;
    pTreeCopyFcn copyInfo;
    pTreeFreeFcn freeInfo;
} AVLTREE;

/*-----------   Function Prototypes  -----------*/

AVLTREE *AvlTreeAlloc(pTreeCopyFcn copyInfo, pTreeFreeFcn freeInfo);

awk_value_t* AvlTreeInsert(AVLTREE *, char* key, awk_value_t info); // replaces info if the key already exists

/* O(log n): both lookup and/or delete: returns false if element not found.
** If found and (int)pInfo==1, delete it; otherwise if pInfo != NULL, populate with new info; otherwise just return true.
*/
awk_value_t* AvlTreeLookDel(AVLTREE *, char* key, awk_value_t* pInfo);
#define AvlTreeDelete(T,k,f) AvlTreeLookDel((T),(k),(awk_value_t*)1)
#define AvlTreeLookup(T,k,p) AvlTreeLookDel((T),(k),(p))
// return NULL = not found

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
