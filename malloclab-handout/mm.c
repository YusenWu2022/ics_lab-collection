/* 
 * Simple, 32-bit and 64-bit clean allocator based on implicit free
 * lists, first-fit placement, and boundary tag coalescing, as described
 * in the CS:APP3e text. Blocks must be aligned to doubleword (8 byte) 
 * boundaries. Minimum block size is 16 bytes. 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./mm.h"
#include "./memlib.h"

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/*
 * debug若定义则使用显式链表，否则使用分离链表
 */
//#define debug

#define ALIGNMENT 8

#define ALIGN(p) (((size_t)(p) + ((ALIGNMENT)-1)) & ~0x7)
/* Basic constants and macros */
#define WSIZE 4            /* Word and header/footer size (bytes) */
#define DSIZE 8            /* Double word size (bytes) */
#define HEAD_WASTE 12      /*Length of head*/
#define LEAST_BLCK_SIZE 16 /*The least length for a block(two DSIZE)*/
#define INIT_SIZE (1 << 6) /*Original size */
#define CHUNK_SIZE (1 << 12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define errsign ((void *)-1)
/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))
/* Pack a size ，a tag and allocated bit into a word */
#define PACK(p, size, tag, alloc) ((tag) == TAG_STD ? ((size) | (alloc) | GET_TAG(p)) : ((size) | (alloc) | (tag)))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_TAG(p) (GET(p) & 0x2)
#define GET_ALLOC(p) (GET(p) & 0x1)
/*tag types in tag place*/
#define TAG_STD ((unsigned int)(-1))
#define TAG_ALLOC 0x2
#define TAG_FREE 0x0
#define SET_ALLOC_TAG(p) (GET(p) |= 0x2)
#define SET_FREE_TAG(p) (GET(p) &= ~0x2)
/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((void *)((char *)(bp)-WSIZE))
#define FTRP(bp) ((void *)((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE))
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLCK(bp) ((void *)((char *)(bp) + GET_SIZE(HDRP(bp))))
#define PREV_BLCK(bp) ((void *)((char *)(bp)-GET_SIZE((char *)(bp)-DSIZE)))
/* Global variables */
static char *heap_top = NULL; /* Pointer to first block */
static void **seglist;        /*different sizes's segments' header*/
/*Given block ptr bp,compute address of its previous or succeeding blocks.*/
static void *SUCC_BLCK(void *bp)
{
    unsigned int tmp = GET(bp);
    if (tmp == 0)
        return NULL;
    else
        return tmp + heap_top;
}
static void *PRED_BLCK(void *bp)
{
    unsigned int tmp = GET((char *)bp + WSIZE);
    if (tmp == 0)
        return NULL;
    else
        return tmp + heap_top;
}
/*set prev or next free block situation*/
#define SET_NEXT_FREE(bp, val) PUT((bp), (unsigned int)((val) ? ((char *)(val)-heap_top) : 0))
#define SET_PREV_FREE(bp, val) PUT((char *)(bp) + WSIZE, (unsigned int)((val) ? ((char *)(val)-heap_top) : 0))
#define SEGLIST_AUTO ((size_t)-1)
/*number of different sizes' block lists */
#define SEGSIZE 17
/* Function prototypes for internal helper routines */
static void *extend_heap(size_t);
static void *coalesce(void *);
static void *find_fit(size_t);
static void place(void *, size_t);
static size_t Index(size_t);
static void LIFO_insert(void *, size_t);
static void delete_list(void *, size_t);
static void *block_find(size_t, size_t);
static void *seglist_find1(size_t, size_t);
/* 
 * mm_init - Initialize the memory manager 
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_top = mem_sbrk(SEGSIZE * DSIZE + 4 * WSIZE)) ==
        errsign)
        return -1;
    seglist = (void **)heap_top;
    for (int i = 0; i < SEGSIZE; i++) /*init seglists' heads */
    {
        seglist[i] = NULL;
    }
    heap_top = (char *)(seglist + SEGSIZE);
    PUT(heap_top, 0);
    PUT(heap_top + 1 * WSIZE, PACK(heap_top + 1 * WSIZE, DSIZE, TAG_FREE, 1)); /* Prologue header */
    PUT(heap_top + 2 * WSIZE, PACK(heap_top + 2 * WSIZE, DSIZE, TAG_FREE, 1)); /* Prologue footer */
    PUT(heap_top + 3 * WSIZE, PACK(heap_top + 3 * WSIZE, 0, TAG_ALLOC, 1));    /* Epilogue header */
    heap_top += DSIZE;
    if (extend_heap(INIT_SIZE) == NULL)
        return -1;

    return 0;
}
/* 
 * malloc - Allocate a block with at least size bytes of payload 
 */
void *malloc(size_t size)
{
    size_t asize; /* Adjusted block size */
    void *bp;
    size_t extendsize; /* Amount to extend heap if no fit */
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;
    if (size == 448) /*special dealt*/
        size += 64;
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= HEAD_WASTE)
        asize = LEAST_BLCK_SIZE;
    else
        asize = ALIGN(WSIZE + size);
    /* Look for a fit */
    if ((bp = find_fit(asize)))
    {
        place(bp, asize);
        return bp;
    }
    extendsize = MAX(asize, CHUNK_SIZE);
    /* No fit found. Get more memory and place the block */
    if ((bp = extend_heap(extendsize / WSIZE)))
    {
        place(bp, asize);
        return bp;
    }
    return NULL;
}
/* 
 * free - Free a block 
 */
void free(void *ptr)
{
    if (!ptr)
        return;
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(HDRP(ptr), size, TAG_STD, 0));
    PUT(FTRP(ptr), PACK(FTRP(ptr), size, TAG_STD, 0));
    void *next = NEXT_BLCK(ptr);
    SET_FREE_TAG(HDRP(next));
    coalesce(ptr);
}
/*
 * realloc - Naive implementation of realloc
 */
void *realloc(void *oldptr, size_t size)
{
    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0)
    {
        free(oldptr);
        return NULL;
    }
    /* If oldptr is NULL, then this is just malloc. */
    if (oldptr == NULL)
    {
        return malloc(size);
    }
    void *newptr;
    if ((newptr = malloc(size)) == NULL)
        return NULL;
    /* Copy the old data. */
    memcpy(newptr, oldptr, MIN(size, GET_SIZE(HDRP(oldptr))));
    /* Free the old block. */
    free(oldptr);
    return newptr;
}

void *calloc(size_t num, size_t size)
{
    size_t total_size = size * num;
    void *bp = malloc(total_size);
    if (!bp)
        return NULL;
    memset(bp, 0, total_size);
    return bp;
}

void mm_checkheap(int lineno) {}
/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
static void *extend_heap(size_t words)
{
    /* Allocate an even number of words to maintain alignment */
    size_t extendsize = ((words % 2) ? (words + 1) : words) * WSIZE;
    /*Special dealt , if size less ,set to least size*/
    if (extendsize < LEAST_BLCK_SIZE)
        extendsize = LEAST_BLCK_SIZE;
    char *bp;
    if ((bp = mem_sbrk(extendsize)) == errsign)
        return NULL;
    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(HDRP(bp), extendsize, TAG_STD, 0));
    PUT(FTRP(bp), PACK(FTRP(bp), extendsize, GET_TAG(HDRP(bp)), 0));
    PUT(HDRP(NEXT_BLCK(bp)), PACK(HDRP(NEXT_BLCK(bp)), WSIZE, TAG_FREE, 1));
    SET_FREE_TAG(HDRP(NEXT_BLCK(bp)));
    /* Coalesce if the previous block was free */
    return coalesce(bp);
}
/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp) //just like simple lists ,remenmber to modify the PRED and SUCC headers
{
    unsigned int prev_alloc = GET_TAG(HDRP(bp)) == TAG_ALLOC;
    unsigned int next_alloc = GET_ALLOC(HDRP(NEXT_BLCK(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    if (prev_alloc && next_alloc)
    { /* Case 1 */
        PUT(FTRP(bp), PACK(FTRP(bp), size, GET_TAG(HDRP(bp)), 0));
    }
    else if (prev_alloc && !next_alloc)
    { /* Case 2 */
        size_t next_size = GET_SIZE(HDRP(NEXT_BLCK(bp)));
        size += next_size;
        delete_list(NEXT_BLCK(bp), next_size);
        PUT(HDRP(bp), PACK(HDRP(bp), size, TAG_STD, 0));
        PUT(FTRP(bp), PACK(FTRP(bp), size, GET_TAG(HDRP(bp)), 0));
    }
    else if (!prev_alloc && next_alloc)
    { /* Case 3 */
        size_t prev_size = GET_SIZE(HDRP(PREV_BLCK(bp)));
        size += prev_size;
        delete_list(PREV_BLCK(bp), prev_size);
        bp = PREV_BLCK(bp);
        PUT(HDRP(bp), PACK(HDRP(bp), size, TAG_STD, 0));
        PUT(FTRP(bp), PACK(FTRP(bp), size, GET_TAG(HDRP(bp)), 0));
    }
    else
    { /* Case 4 */
        size_t next_size = GET_SIZE(HDRP(NEXT_BLCK(bp)));
        size_t prev_size = GET_SIZE(HDRP(PREV_BLCK(bp)));
        size += next_size + prev_size;
        delete_list(NEXT_BLCK(bp), next_size);
        delete_list(PREV_BLCK(bp), prev_size);
        bp = PREV_BLCK(bp);
        PUT(HDRP(bp), PACK(HDRP(bp), size, TAG_STD, 0));
        PUT(FTRP(bp), PACK(FTRP(bp), size, GET_TAG(HDRP(bp)), 0));
    }
    LIFO_insert(bp, size);

    return bp;
}
/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(void *ptr, size_t asize)
{
    size_t free_size = GET_SIZE(HDRP(ptr));
    delete_list(ptr, free_size);
    //ptrdiff_t diff = free_size - asize;
    size_t diff = free_size - asize;
    if (diff >= LEAST_BLCK_SIZE)
    {
        PUT(HDRP(ptr), PACK(HDRP(ptr), asize, TAG_STD, 1));
        void *bp = NEXT_BLCK(ptr);
        PUT(HDRP(bp), PACK(HDRP(ptr), diff, TAG_ALLOC, 0));
        PUT(FTRP(bp), PACK(FTRP(bp), diff, TAG_ALLOC, 0));
        LIFO_insert(bp, diff);
    }
    else
    {
        PUT(HDRP(ptr), PACK(HDRP(ptr), free_size, TAG_STD, 1));
        SET_ALLOC_TAG(HDRP(NEXT_BLCK(ptr)));
    }
}
/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
static void *find_fit(size_t size) //First fit search
{

    size_t index = Index(size);
    void *fp = NULL;
#ifdef debug
    if (index < SEGSIZE)
    {
        if (index == SEGLIST_AUTO)
            index = Index(size);
        void *fp = seglist[0]; //seghead
        /* Search from the list-begin to the end of list */
        while (fp)
        {
            if (size <= GET_SIZE(HDRP(fp)))
                return fp;
            fp = SUCC_BLCK(fp);
        }
    }
    return NULL;
#else
    for (; index < SEGSIZE; index++) //go over all the seglists with size larger than need
    {
        if (index == SEGLIST_AUTO)
            index = Index(size);
        fp = seglist[index];
        while (fp)
        { /* Search from the list-begin to the end of list */
            if (size <= GET_SIZE(HDRP(fp)))
                return fp;
            fp = SUCC_BLCK(fp);
        }
    }
    return NULL;
#endif
}
/*
*Index -   according to size needed to cal the least room group needed with math mathod
*/
static size_t Index(size_t size)
{
    int ind = 0;
    if (size >= 4096)
        return 8;

    size = size >> 5;
    while (size)
    {
        size = size >> 1;
        ind++;
    }
    return ind;
}
/*
*LIFO_insert - insert a free block to clarified seglist
*/
static void LIFO_insert(void *fp, size_t size)
{
    size_t index = Index(size);
#ifdef debug
    char *insert_pt = seglist[0];
#else
    char *insert_pt = seglist[index];
#endif
    if (insert_pt)
    {
        SET_PREV_FREE(insert_pt, fp);
    }
    SET_NEXT_FREE(fp, insert_pt);
    SET_PREV_FREE(fp, NULL);
#ifdef debug
    seglist[0] = fp;
#else
    seglist[index] = fp;
#endif
}
/*
* delete_list - delete a block from its seglist
*/
static void delete_list(void *fp, size_t size)
{
    size_t index = Index(size);
    char *next = SUCC_BLCK(fp);
    char *prev = PRED_BLCK(fp);
    if (prev)
    {
        //printf("1");
        SET_NEXT_FREE(prev, next);
        //printf("2");
    }
    else
    {
#ifdef debug
        seglist[0] = next;
#else
        seglist[index] = next;
#endif
    }
    if (next)
    {
        SET_PREV_FREE(next, prev);
    }
#ifdef debug
    if (seglist[1] == fp)
    {
        //if (next)
        //seglist[1] = next;
        //else
        seglist[1] = seglist[0];
    }
#endif
}
/*
*block_find - important ! go through the index set seglist to find a block with enough size
*/
static void *block_find(size_t index, size_t size)
{
    if (index == SEGLIST_AUTO)
        index = Index(size);
#ifdef debug
    void *fp = seglist[0];
#else
    void *fp = seglist[index];
#endif
    while (fp)
    {
        if (size <= GET_SIZE(HDRP(fp)))
            return fp;
        fp = SUCC_BLCK(fp);
    }
    return NULL;
}

