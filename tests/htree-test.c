#include "misc.h"
#include "htree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Usage: ./htree-test htree-test.in
Enter 3 keys per line to lookup; EOF to exit*/

#define DEPTH 3
int main(int argc, char *argv[])
{
    assert(argc == 2);
{
    FILE *fp = fopen(argv[1], "r");
    HTREE *h = HTreeAlloc(DEPTH, (pCmpFcn)strcmp, (pFointCopyFcn)strdup, (pFointFreeFcn)free, NULL, NULL);
    char buf[DEPTH][BUFSIZ];
    char *keys[DEPTH];
    foint data;
    int i;
    for(i=0;i<DEPTH;i++) keys[i] = buf[i];

    while(fscanf(fp, "%s%s%s%d", keys[0], keys[1], keys[2], &data.i) == 4) {
	printf("Inserting %s %s %s = %d\n", keys[0], keys[1], keys[2], data.i);
	HTreeInsert(h, (foint*)keys, (foint)data);
	foint* found = HTreeLookup(h, (foint*)keys);
	if(found == NULL) {
	    fprintf(stderr, "For key ");
	    for(i=0;i<DEPTH;i++) fprintf(stderr, "[%s]", keys[i]);
	    Fatal("HTreeLookup couldn't find an entry");
	}
	
	assert(found->i == data.i);
    }
    fclose(fp);

    while(scanf("%s %s %s", keys[0], keys[1], keys[2]) == 3)
    {
	int sizes[DEPTH]={-1,-1,-1};
	int depth = HTreeSizes(h, (foint*)keys, sizes);
	printf("filled %d sizes [%d %d %d]\n",depth, sizes[0],sizes[1],sizes[2]);
	foint* found = HTreeLookup(h, (foint*)keys);
	if(found != NULL)
	    printf("%d\n", found->i);
	else
	    puts("not found");
    }

    HTreeFree(h);
    return 0;
}
}
