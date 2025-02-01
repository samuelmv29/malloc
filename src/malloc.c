#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)     ((b) + 1)
#define BLOCK_HEADER(ptr) ((struct _block *)(ptr) - 1)

static int atexit_registered = 0; //DONE
static int num_mallocs       = 0; //DONE
static int num_frees         = 0; //DONE
static int num_reuses        = 0; //DONE
static int num_grows         = 0; //DONE
static int num_splits        = 0; //DONE
static int num_coalesces     = 0; //DONE
static int num_blocks        = 0; //DONE
static int num_requested     = 0; //DONE
static int max_heap          = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics( void )
{
  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

struct _block 
{
   size_t  size;         /* Size of the allocated _block of memory in bytes */
   struct _block *next;  /* Pointer to the next _block of allocated memory  */
   bool   free;          /* Is this _block free?                            */
   char   padding[3];    /* Padding: IENTRTMzMjAgU3jMDEED                   */
};


struct _block *heapList = NULL; /* Free list to track the _blocks available */

/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes 
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 ------------------ DONE -----------------
 * \TODO Implement Best Fit
 --------------- DONE --------------------
 * \TODO Implement Worst Fit
 ----------------- DONE -------------------
 */
struct _block *findFreeBlock(struct _block **last, size_t size) 
{
   struct _block *curr = heapList;

#if defined FIT && FIT == 0
   /* First fit */
   //
   // While we haven't run off the end of the linked list and
   // while the current node we point to isn't free or isn't big enough
   // then continue to iterate over the list.  This loop ends either
   // with curr pointing to NULL, meaning we've run to the end of the list
   // without finding a node or it ends pointing to a free node that has enough
   // space for the request.
   // 
   while (curr && !(curr->free && curr->size >= size)) 
   {
      *last = curr;
      curr  = curr->next;
   }
#endif

// \TODO Put your Best Fit code in this #ifdef block
#if defined BEST && BEST == 0
   /** \TODO Implement best fit here */
   struct _block *bestfit = NULL;
  // struct _block *temp_curr = heapList;

   while (curr!=NULL)
   {
      if ((curr->free && curr->size >= size) && (bestfit == NULL || curr->size < bestfit->size))
      {
         bestfit = curr;
      }
      *last = curr;
      curr  = curr->next;
   }
   curr = bestfit;
   
#endif

// \TODO Put your Worst Fit code in this #ifdef block
#if defined WORST && WORST == 0
   
   /*while (curr && !(curr->free && curr->size >= size && curr->size-size >= size_diff))
   {
      *last = curr;
      curr  = curr->next;
   }  */
   struct _block *worstfit = NULL;
   while (curr!=NULL)
   {
      if ((curr->free && curr->size >= size) && (worstfit == NULL || curr->size > worstfit->size))
      {
         worstfit = curr;
      }
      *last = curr;
      curr  = curr->next;
   }
   curr = worstfit;
#endif

// \TODO Put your Next Fit code in this #ifdef block
#if defined NEXT && NEXT == 0
   static struct _block *lastly = NULL;
   struct _block *current = NULL;
   if (lastly != NULL)
   {
      current = lastly->next;
   }
   else
   {
      current = heapList;
   }

   

   while (current!= NULL && !(current->free && current->size >= size)) 
   {
      *last = current;
      current  = current->next;
   }
   
   
   if (current != NULL)
   {
         lastly = current;
   }
   else
   {
      lastly = heapList;
   }
   curr = current;
#endif

   
   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically 
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size) 
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1) 
   {
      return NULL;
   }

   /* Update heapList if not set */
   if (heapList == NULL) 
   {
      heapList = curr;
   }

   /* Attach new _block to previous _block */
   if (last) 
   {
      last->next = curr;
   }

   /* Update _block metadata:
      Set the size of the new block and initialize the new block to "free".
      Set its next pointer to NULL since it's now the tail of the linked list.
   */
   curr->size = size;
   curr->next = NULL;
   curr->free = false;

   //NUM_BLOCKS
   num_blocks ++;   
   //NUM_GROWS
   num_grows += 1;
   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the 
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process 
 * or NULL if failed
 */
void *malloc(size_t size) 
{

   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0) 
   {
      return NULL;
   }
   
   //NUM_REQUESTED
   num_requested += size;

   /* Look for free _block.  If a free block isn't found then we need to grow our heap. */

   struct _block *last = heapList;
   struct _block *next = findFreeBlock(&last, size);

   if (next != NULL)
   {
      //NUM_BLOCKS
      num_blocks ++;
   

   /* TODO: If the block found by findFreeBlock is larger than we need then:
            If the leftover space in the new block is greater than the sizeof(_block)+4 then
            split the block.
            If the leftover space in the new block is less than the sizeof(_block)+4 then
            don't split the block.
   ----------------- DONE ------------------------
   */

      size_t leftover_space = next->size - size;
      if (leftover_space >= sizeof(struct _block) + 4) 
      {
         //NUM_SPLITS
         num_splits++;

         /*//NUM_COALESCES
         num_coalesces++;*/
         struct _block *new_block = (struct _block *)((char *)next + sizeof(struct _block) + size);
         new_block->size = leftover_space - sizeof(struct _block);
         new_block->next = next->next;
         new_block->free = true;
         next->size = size;

         if (last != NULL) 
         {
            last->next = new_block;
         }
         else 
         {
            heapList = new_block;
         }
      }
   }
   if (next == NULL) 
   {
      next = growHeap(last, size);
   }

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL) 
   {
      return NULL;
   }
   
   /* Mark _block as in use */
   next->free = false;

   /* Return data address associated with _block to the user */
   //NUM_MALLOCS
   num_mallocs += 1;
   return BLOCK_DATA(next);
   struct _block *temp = heapList;
   while (temp != NULL) {
      max_heap += temp->size + sizeof(struct _block);
   
      temp = temp->next;
   }
}

/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr) 
{
   if (ptr == NULL) 
   {
      return;
   }

   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;

   num_blocks--;

   /* TODO: Coalesce free _blocks.  If the next block or previous block 
            are free then combine them with this block being freed.
   ---------------------- DONE ---------------------
   */
   struct _block *prev = heapList;
   while (prev->next != curr && prev->next != NULL) {
      prev = prev->next;
   }

   if (curr->next != NULL && curr->next->free) {
      curr->size += sizeof(struct _block) + curr->next->size;
      curr->next = curr->next->next;

      num_coalesces++;
   }
   if (prev != NULL && prev->free) {
      prev->size += sizeof(struct _block) + curr->size;
      prev->next = curr->next;

      num_coalesces++;
   }

   // Check if the current block and either the next block or the previous block
   // have been merged during coalescing, indicating a reuse
   if ((curr->next != NULL && (char *)curr + sizeof(struct _block) + curr->size == (char *)curr->next) ||
       (prev != NULL && (char *)prev + sizeof(struct _block) + prev->size == (char *)curr)) {
      num_reuses++;
   }

    num_frees++;
   //NUM_FREES
   num_frees += 1;
   struct _block *temp = heapList;
   while (temp != NULL) {
      max_heap += temp->size + sizeof(struct _block);
   
      temp = temp->next;
   }
}

void *calloc( size_t nmemb, size_t size )
{
   // \TODO Implement calloc
   /* Calculate the total size to allocate 
   ------------- DONE --------------------
   */
    size_t total_size = nmemb * size;

    /* Allocate memory using malloc */
    void *ptr = malloc(total_size);

    /* Check if memory allocation was successful */
    if (ptr != NULL) 
    {
        /* Initialize the allocated memory to zero */
        memset(ptr, 0, total_size);
    }
   struct _block *temp = heapList;
   while (temp != NULL) {
      max_heap += temp->size + sizeof(struct _block);
   
      temp = temp->next;
   }
    /* Return the pointer to the allocated memory */
    return ptr;
}

void *realloc( void *ptr, size_t size )
{
   // \TODO Implement realloc
   /* ------------------- DONE -------------- */
   if (ptr == NULL) 
   {
      // If ptr is NULL, realloc behaves like malloc
      return malloc(size);
   }

   if (size == 0) 
   {
      // If size is 0, realloc behaves like free
      free(ptr);
      return NULL;
   }

   // Reallocate memory using malloc and memcpy
   void *new_ptr = malloc(size);
   if (new_ptr != NULL) 
   {
      // Copy data from old block to new block
      struct _block *old_block = BLOCK_HEADER(ptr);
      size_t old_size = old_block->size;
      size_t copy_size = old_size < size ? old_size : size;
      memcpy(new_ptr, ptr, copy_size);
      // Free the old block
      free(ptr);
   }
   struct _block *temp = heapList;
   while (temp != NULL) {
      max_heap += temp->size + sizeof(struct _block);
   
      temp = temp->next;
   }
   return new_ptr;
}



/* vim: IENTRTMzMjAgU3ByaW5nIDIwMjM= -----------------------------------------*/
/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
