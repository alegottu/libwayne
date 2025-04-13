#ifdef __cplusplus
extern "C" {
#endif
#include "misc.h"
#include "htree.h" // bintree or avltree is included there (as appropriate)

/*-------------------  Types  ------------------*/

HTREE *HTreeAlloc(int depth, pTreeCmpFcn cmpKey, pTreeCopyFcn copyKey, pTreeFreeFcn freeKey,
    pTreeCopyFcn copyInfo, pTreeFreeFcn freeInfo)
{
    assert(depth>0);
    HTREE *h = Calloc(1, sizeof(HTREE));
    h->depth = depth;
    // Be nice and create the top-level tree.
    h->cmpKey = cmpKey; h->copyKey = copyKey; h->freeKey = freeKey;
    h->copyInfo = copyInfo; h->freeInfo = freeInfo;

	// Intermediary trees only store pointers, default copy and free should be used
	if (depth != 1)
		copyInfo = NULL; freeInfo = NULL;

    h->tree = TreeAlloc(cmpKey, copyKey, freeKey, copyInfo, freeInfo);
    return h;
}

static awk_value_t* HTreeInsertHelper(HTREE *h, int currentDepth, TREETYPE *tree, awk_value_t keys[], awk_value_t data)
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    if(currentDepth == h->depth-1) // we're hit the lowest level tree; its data elements are the final elements.
	return TreeInsert(tree, keys[currentDepth], data);
    else {
	// Otherwise, we are NOT at the lowest level tree; the data members of these nodes are themselves other trees,
	// so to find the next tree we use the key at this level to *look up* the binary tree at the next level down
	awk_value_t nextLevel;
	TREETYPE *nextTree;
	if(!TreeLookup(tree, keys[currentDepth], &nextLevel)) {
		pTreeCopyFcn copyInfo = h->copyInfo;
		pTreeFreeFcn freeInfo = h->freeInfo;

		// Intermediary trees only store pointers, default copy and free should be used
		if (currentDepth < (h->depth-2))
			copyInfo = NULL; freeInfo = NULL;

	    nextTree = TreeAlloc(h->cmpKey, h->copyKey, h->freeKey, copyInfo, freeInfo);
	    nextLevel.num_ptr = nextTree;
	    TreeInsert(tree, keys[currentDepth], nextLevel);
	}
	else
	    nextTree = nextLevel.num_ptr;
	assert(nextTree);
	HTreeInsertHelper(h, currentDepth+1, nextTree, keys, data);
    }
	return NULL;
}

// key is an array with exactly "depth" elements, data is what you want to put at the lowest level.
awk_value_t* HTreeInsert(HTREE *h, awk_value_t keys[], awk_value_t data)
{
	// TODO: not sure if this first line is really necessary
    awk_value_t fkeys[h->depth]; int i; for(i=0; i < h->depth; i++) fkeys[i] = keys[i];
    return HTreeInsertHelper(h, 0, h->tree, fkeys, data);
}

// TODO: edit LookDel to be possible to stop before final level
static awk_value_t* HTreeLookDelHelper(HTREE *h, int currentDepth, TREETYPE *tree, awk_value_t keys[], awk_value_t *pData)
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    if(currentDepth == h->depth-1) // we've hit the lowest level tree; its data elements are the final elements.
	return TreeLookDel(tree, keys[currentDepth], pData);
    else {
	awk_value_t nextLevel;
	TREETYPE *nextTree;
	if(!TreeLookup(tree, keys[currentDepth], &nextLevel))
	    return NULL;
	else
	    nextTree = nextLevel.num_ptr;
	assert(nextTree);
	return HTreeLookDelHelper(h, currentDepth+1, nextTree, keys, pData);
    }
}

awk_value_t* HTreeLookDel(HTREE *h, awk_value_t keys[], awk_value_t *pData)
{
	// TODO: same as with insert
    awk_value_t fkeys[h->depth]; int i; for(i=0; i < h->depth; i++) fkeys[i] = keys[i];
    return HTreeLookDelHelper(h, 0, h->tree, fkeys, pData);
}

static int HTreeSizesHelper(HTREE *h, int currentDepth, TREETYPE *tree, awk_value_t keys[], int sizes[])
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    sizes[currentDepth] = tree->n;
    if(currentDepth == h->depth-1) // we're hit the lowest level tree; its data elements are the final elements.
	return 1;
    else {
	awk_value_t nextLevel;
	TREETYPE *nextTree;
	if(!TreeLookup(tree, keys[currentDepth], &nextLevel))
	    return 1;
	else
	    nextTree = nextLevel.num_ptr;
	assert(nextTree);
	return 1 + HTreeSizesHelper(h, currentDepth+1, nextTree, keys, sizes);
    }
}

int HTreeSizes(HTREE *h, awk_value_t keys[], int sizes[])
{
	// TODO: same as with LookDel
    awk_value_t fkeys[h->depth]; int i; for(i=0; i < h->depth; i++) fkeys[i] = keys[i];
    return HTreeSizesHelper(h, 0, h->tree, fkeys, sizes);
}


static void HTreeFreeHelper(foint globals, HTREE *h, int currentDepth, TREETYPE *tree);
static HTREE *_TraverseH;
static int _TraverseDepth;
// TODO: also not sure if need to change foint here / just above
static int TraverseFree(foint globals, awk_value_t key, awk_value_t data) {
    assert(_TraverseDepth < _TraverseH->depth);
    TREETYPE *t = data.num_ptr;
    int depth = _TraverseDepth;
    HTreeFreeHelper(globals, _TraverseH, _TraverseDepth+1, t);
    _TraverseDepth = depth;
    return 1;
}

static void HTreeFreeHelper(foint globals, HTREE *h, int currentDepth, TREETYPE *tree)
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    if(currentDepth == h->depth-1) // we're hit the lowest level tree; its data elements are the final elements.
	TreeFree(tree);
    else {
	_TraverseH = h; _TraverseDepth = currentDepth;
	TreeTraverse(globals, tree, (pTreeTraverseFcn) TraverseFree);
    }
}

void HTreeFree(HTREE *h)
{
	foint globals; globals.v = NULL;
    HTreeFreeHelper(globals, h, 0, h->tree);
}
#ifdef __cplusplus
} // end extern "C"
#endif
