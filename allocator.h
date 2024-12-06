#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

/* Basic constants and macros */
#define WSIZE       4              /* Word and header/footer size (bytes) */
#define DSIZE       8              /* Double word size (bytes) */
#define CHUNKSIZE   (1 << 12)      /* Extend heap by this amount (bytes) */

#define MAX(x, y)   ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)     ((char *)(bp) - WSIZE)
#define FTRP(bp)     ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define NEXT_FREE_BLOCK(bp)  (*(char **)(bp))
#define PREV_FREE_BLOCK(bp)  (*(char **)((char *)(bp) + WSIZE))

/* Function prototypes */
void *mem_sbrk(int incr);
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *extend_heap(size_t words);
void *coalesce(void *bp);
void place(void *bp, size_t asize);
void *find_fit(size_t asize);
#endif
