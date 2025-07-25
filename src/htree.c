#ifdef __cplusplus
extern "C" {
#endif
#include "misc.h"
#include "htree.h" // bintree or avltree is included there (as appropriate)

/*-------------------  Types  ------------------*/

HTREE *HTreeAlloc(unsigned char depth, pCmpFcn cmpKey, pFointCopyFcn copyKey, pFointFreeFcn freeKey,
    pFointCopyFcn copyInfo, pFointFreeFcn freeInfo)
{
    assert(depth>0);
    HTREE *h = Calloc(1, sizeof(HTREE));
    h->depth = depth;
    // Be nice and create the top-level tree.
    h->cmpKey = cmpKey; h->copyKey = copyKey; h->freeKey = freeKey;
    h->copyInfo = copyInfo; h->freeInfo = freeInfo;

	// Intermediary trees only store pointers, default copy and free should be used
	if (depth != 1)
	{
		copyInfo = NULL; freeInfo = NULL;
	}

    h->tree = TreeAlloc(cmpKey, copyKey, freeKey, copyInfo, freeInfo);
    return h;
}

static foint* const HTreeInsertHelper(HTREE *h, unsigned char currentDepth, TREETYPE *tree, foint keys[], foint data)
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    if(currentDepth == h->depth-1) // we're hit the lowest level tree; its data elements are the final elements.
		return TreeInsert(tree, keys[currentDepth], data);
    else {
		// Otherwise, we are NOT at the lowest level tree; the data members of these nodes are themselves other trees,
		// so to find the next tree we use the key at this level to *look up* the binary tree at the next level down
		foint* nextLevel = TreeLookup(tree, keys[currentDepth]);
		TREETYPE *nextTree;
		if(nextLevel == NULL) {
			pFointCopyFcn copyInfo = h->copyInfo;
			pFointFreeFcn freeInfo = h->freeInfo;

			// Intermediary trees only store pointers, default copy and free should be used
			if (currentDepth < (h->depth-2))
			{
				copyInfo = NULL; freeInfo = NULL;
			}

			nextTree = TreeAlloc(h->cmpKey, h->copyKey, h->freeKey, copyInfo, freeInfo);
			TreeInsert(tree, keys[currentDepth], (foint){.v=nextTree});
		}
		else
			nextTree = nextLevel->v;
		assert(nextTree);
		return HTreeInsertHelper(h, currentDepth+1, nextTree, keys, data);
    }
}

// key is an array with exactly "depth" elements, data is what you want to put at the lowest level.
foint* const HTreeInsert(HTREE *h, foint keys[], foint data)
{
    foint fkeys[h->depth]; int i; for(i=0; i < h->depth; i++) fkeys[i] = keys[i];
    return HTreeInsertHelper(h, 0, h->tree, fkeys, data);
}

static foint* HTreeLookDelHelper(HTREE *h, unsigned char currentDepth, TREETYPE *tree, foint keys[], unsigned char targetDepth, Boolean delete)
{
    assert(tree && 0 <= currentDepth && currentDepth < targetDepth);
    if(currentDepth == targetDepth-1)
	{
		if (delete && targetDepth < h->depth)
		{
			if (AvlTreeSpecialDel(tree, keys[currentDepth], (pFointFreeFcn)TreeFree)) return (foint*)1;
			else return NULL;
		}
		else return TreeLookDel(tree, keys[currentDepth], delete);
	}
    else {
	foint* nextLevel = TreeLookup(tree, keys[currentDepth]);
	TREETYPE *nextTree;
	if(nextLevel == NULL)
	    return NULL;
	else
	    nextTree = nextLevel->v;
	assert(nextTree);
	return HTreeLookDelHelper(h, currentDepth+1, nextTree, keys, targetDepth, delete);
    }
}

foint* HTreeLookDel(HTREE *h, foint keys[], unsigned char targetDepth, Boolean delete)
{
	assert(targetDepth <= h->depth);
	targetDepth = targetDepth < 0 ? h->depth : targetDepth;
    foint fkeys[h->depth]; int i; for(i=0; i < h->depth; i++) fkeys[i] = keys[i];
    return HTreeLookDelHelper(h, 0, h->tree, fkeys, targetDepth, delete);
}

const Boolean SHTreeLookup(HTREE* tree, foint keys[], unsigned char targetDepth, foint* pInfo)
{
	foint* result = HTreeLookDel(tree, keys, targetDepth, false);

	if (result != NULL) 
	{
		if (pInfo != NULL)
			*pInfo = *result;
		return true;
	}
	else
		return false;
}

static int HTreeSizesHelper(HTREE *h, unsigned short currentDepth, TREETYPE *tree, foint keys[], int sizes[])
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    sizes[currentDepth] = tree->n;
    if(currentDepth == h->depth-1) // we're hit the lowest level tree; its data elements are the final elements.
	return 1;
    else {
	foint* nextLevel = TreeLookup(tree, keys[currentDepth]);
	TREETYPE *nextTree;
	if(nextLevel == NULL)
	    return 1;
	else
	    nextTree = nextLevel->v;
	assert(nextTree);
	return 1 + HTreeSizesHelper(h, currentDepth+1, nextTree, keys, sizes);
    }
}

int HTreeSizes(HTREE *h, foint keys[], int sizes[])
{
    foint fkeys[h->depth]; int i; for(i=0; i < h->depth; i++) fkeys[i] = keys[i];
    return HTreeSizesHelper(h, 0, h->tree, fkeys, sizes);
}


static void HTreeFreeHelper(foint globals, HTREE *h, unsigned short currentDepth, TREETYPE *tree);
static HTREE *_TraverseH;
static int _TraverseDepth;
static int TraverseFree(foint globals, foint key, foint data) {
    assert(_TraverseDepth < _TraverseH->depth);
    TREETYPE *t = data.v;
    int depth = _TraverseDepth;
    HTreeFreeHelper(globals, _TraverseH, _TraverseDepth+1, t);
    _TraverseDepth = depth;
    return 1;
}

static void HTreeFreeHelper(foint globals, HTREE *h, unsigned short currentDepth, TREETYPE *tree)
{
    assert(tree && 0 <= currentDepth && currentDepth < h->depth);
    if(currentDepth == h->depth-1) // we're hit the lowest level tree; its data elements are the final elements.
		TreeFree(tree);
    else
	{
		_TraverseH = h; _TraverseDepth = currentDepth;
		TreeTraverse(globals, tree, (pFointTraverseFcn) TraverseFree);
		TreeFree(tree);
    }
}

void HTreeFree(HTREE *h)
{
    HTreeFreeHelper((foint)NULL, h, 0, h->tree);
	free(h);
}
#ifdef __cplusplus
} // end extern "C"
#endif
