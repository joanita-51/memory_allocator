#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"
#include <errno.h> //for ENOMEM


#define MAX_HEAP (20 * 1024 * 1024)  // Define the maximum heap size

/* Simulated memory functions */
static char *mem_heap;        /* Points to the start of the heap */
static char *mem_brk;         /* Points to the end of the heap */
static char *mem_max_addr;    /* Maximum address in the heap */

void mem_init(void) {
    mem_heap = (char *)malloc(MAX_HEAP);
    printf("Heap initialized at: %p\n", mem_heap);
    if (mem_heap == NULL) {
        fprintf(stderr, "Failed to initialize memory.\n");
        exit(1);
    }
    mem_brk = mem_heap;
    printf("Memory break initialized at: %p\n", mem_brk);
    mem_max_addr = mem_heap + MAX_HEAP;
    printf("Maximum address is: %p\n", mem_max_addr);

}

void *mem_sbrk(int incr) {
    char *old_brk = mem_brk;

    if ((incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Out of memory...\n");
        return (void *)-1;
    }

    mem_brk += incr;
    return (void *)old_brk;
}



//Demonstrating an allocator
int main(void) {
    /* Initialize memory */
    mem_init();

    /* Initialize allocator */
    if (mm_init() == -1) {
        fprintf(stderr, "Allocator initialization failed.\n");
        return 1;
    }


    /* Example usage */
    void *ptr = mm_malloc(32);
    if (ptr == NULL)
    {
        printf("Memory allocation failed!\n");
    } else {
        printf("Memory allocated at: %p\n",ptr);
    }
    
    mm_free(ptr);


    printf("Memory freed.\n");
    return 0;
}
