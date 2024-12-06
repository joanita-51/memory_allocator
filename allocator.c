#include "allocator.h"
#include <errno.h>
#include <stdio.h>

#define WSIZE 4  /* Word size (bytes) */
#define DSIZE 8  /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Initial heap size */

/* Global variables */
static char *heap_listp = NULL;

/* Memory allocator functions */
int mm_init(void) {
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1) {
        return -1;
    }
    PUT(heap_listp, 0);                                 // Alignment padding
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));      // Prologue header
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));      // Prologue footer
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));          // Epilogue header
    heap_listp += (2 * WSIZE);

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
        return -1;
    }
    return 0;
}

void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

void *mm_malloc(size_t size) {
    size_t asize;
    size_t extendsize;
    char *bp;

    if (size == 0) return NULL;

    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE - 1)) / DSIZE);

    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL) return NULL;
    place(bp, asize);
    return bp;
}

// Helper functions for managing memory

void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        return bp;
    } else if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    } else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    } else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

void *find_fit(size_t asize) {
    void *bp;

    /* Iterate through the free list */
    for (bp = heap_listp; bp != NULL; bp = NEXT_FREE_BLOCK(bp)) {
        if (GET_SIZE(HDRP(bp)) >= asize && !GET_ALLOC(HDRP(bp))) {
            return bp;  // Found a suitable block
        }
    }
    return NULL;  // No fit found
}

void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));  // Current block size

    if ((csize - asize) >= (2 * DSIZE)) {  // Split if the remainder is big enough
        PUT(HDRP(bp), PACK(asize, 1));  // Set header as allocated
        PUT(FTRP(bp), PACK(asize, 1));  // Set footer as allocated

        bp = NEXT_BLKP(bp);  // Move to the next block
        PUT(HDRP(bp), PACK(csize - asize, 0));  // Update header of the new free block
        PUT(FTRP(bp), PACK(csize - asize, 0));  // Update footer of the new free block
    } else {  // Do not split, allocate the entire block
        PUT(HDRP(bp), PACK(csize, 1));  // Set header as allocated
        PUT(FTRP(bp), PACK(csize, 1));  // Set footer as allocated
    }
}
