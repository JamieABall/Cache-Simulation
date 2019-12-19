
//Declare Variables
 int mm_size = 0;
 int cache_size = 0;
 int block_size = 0;
 int have_params = 0;
 
#include <stdio.h>
#include <stdlib.h>

//Define Structures
typedef struct Node {
   int tag;
   int* block;
} Node;

typedef struct MainMemory {
   int size;
   int* data;
} MainMemory;

Node* cache = NULL;
MainMemory mm; 


//Procedure to Print Menu
void printMenuHeader() {
  printf("\nMain Menu - Main Memory to Cache Memory Mapping\n");
  printf("------------------------------------------------\n");
  printf("1) Enter Configuration Parameters\n");
  printf("2) Read from Cache\n");
  printf("3) Write to Cache\n");
  printf("4) Quit Program\n\n");
}

//Procedure to Free Allocated Memory
//
void freeMainMemory(struct MainMemory* mainMemory) {
  if (mainMemory->data) {
    free(mainMemory->data);
  }
}

//Procedure to Free Allocated Cache Blocks
void free_cache(struct Node* cache) {
   if(cache) {
      for(int i = 0; i < cache_size; i++) {
         free(cache[i].block); //free all the blocks
      }
   }
   free(cache); //free the cache itself
}

//Procedure to Check for Power of 2
int check_pow_two(int num) {
   if(!num) { // if the number is zero it's not a power of 2
      return 0;
   }
   int scaler = 1; //start at the lowest power of 2
      
   while(scaler < num) {
      scaler *= 2; //while we're less than @param num multiply
   }
      
   if(scaler == num) { //if we end up equaling num we're a power of 2
      return 1;
   }
   return 0; //we're not a power of 2 if we come out greater than num   
}

void enter_params() {
   //just in case parameters are re-entered
   free_cache(cache);
   freeMainMemory(&mm);
   
   //reset size variables
   mm_size = 0;
   cache_size = 0;
   block_size = 0;
   
   printf("Enter main memory size (words):\n");
   scanf("%d", &mm_size);
   
   printf("Enter cache size (words):\n");
   scanf("%d", &cache_size);
   
   printf("Enter block size (words/block):\n");
   scanf("%d", &block_size);
   
  if(!check_pow_two(mm_size)) {
     printf("*** Error - Main Memory Size is not a Power of 2\n");
     return;
  } else if(!check_pow_two(cache_size)) {
      printf("*** Error - Cache Size is not a Power of 2\n");
      return;
  } else if(!check_pow_two(block_size)) {
     printf("*** Error - Block Size is not a Power of 2\n");
     return;
  } else if(block_size > cache_size) {
     printf("*** Error - Block Size is Larger than Cache Size\n");
     return;
  }
   printf("*** All Input Parameters Accepted. Starting to Process Write/Read Requests\n");
   mm.size = mm_size;
   
   //allocate main memory
   mm.data = (int*)malloc(sizeof(int) * mm.size);
   
   for(int i = 0; i < mm.size; i++) {
      mm.data[i] = mm.size - i;
   }
   
   //allocate cache
   cache = (Node*)malloc(sizeof(Node) * cache_size);
   
   for(int j = 0; j < cache_size; j++) {
      cache[j].block = NULL;
      cache[j].tag = 0;
   }
   have_params = 1;
}
void read_cache() {
   int addr = 0;
   int block_number = 0;
   int line_number = 0;
   int tag = 0;
   
   printf("Enter Main Memory Address to read:\n");
   scanf("%d", &addr);
   
   if(addr > mm.size) {
      printf("*** Error – Read Address Exceeds Memory Address Space\n");
      return;
   }
   
   block_number = (addr % cache_size) / block_size;
   line_number = addr % block_size;
   tag = addr / cache_size;
   
   if(cache[block_number].block == NULL || cache[block_number].tag != tag) {
      printf("*** Read Miss - First load block from Memory!\n");
      cache[block_number].block = (int*)malloc(sizeof(int) * block_size);
      cache[block_number].block[line_number] = mm.data[addr];
      cache[block_number].tag = tag;
   } else {
      printf("*** Cache Hit\n");
   }
    printf("*** Word %d of Cache Line %d with Tag %d contains the Value %d\n",
         line_number,
         block_number,
         cache[block_number].tag,
         cache[block_number].block[line_number]);
}
/* Write Cache */
void write_cache() {
   int addr = 0;
   int data = 0;
   int block_number = 0;
   int line_number = 0;
   int tag = 0;
  printf("Enter Main Memory Address to Write:\n");
  scanf("%d", &addr);
  
  if(addr > mm.size) {
     printf("*** Error – Write Address Exceeds Memory Address Space\n");
     scanf("%d", &data);
     return;
  }
  printf("Enter Value to Write:\n");
  scanf("%d", &data);
  
  block_number = (addr % cache_size) / block_size;
  line_number = addr % block_size;
  tag = addr / cache_size;
  
  
  if(cache[block_number].block == NULL || cache[block_number].tag != tag) {
     printf("*** Write Miss - First load block from Memory\n");
     cache[block_number].block = (int*)malloc(sizeof(int) * block_size);
     cache[block_number].block[line_number] = data;
     cache[block_number].tag = tag;
  } else {
     printf("*** Cache Hit\n");
     
     if(cache[block_number].block[line_number]) {
        //write through
        cache[block_number].block[line_number] = data;
        cache[block_number].tag = tag;
        mm.data[addr] = data;
     } else {
        cache[block_number].block[line_number] = data;
        cache[block_number].tag = tag;
     }
  }
  printf(" *** Word %d of Cache Line %d with Tag %d contains the Value %d\n",
         line_number,
         block_number,
         cache[block_number].tag,
         cache[block_number].block[line_number]);
}

/* Main Program */
int main() {
   int choice = 0;
   
   do {
      printMenuHeader();
      printf("Enter selection: \n\n");
      scanf("%d", &choice);
      
      //check to see if parameters were entered
      if(!have_params && (choice != 1 && choice != 4)) {
         printf("*** Error - Invalid Menu Option Selected\n");
         printMenuHeader();
         scanf("%d", &choice);
         printf("Enter Selection:\n\n");
      }

      switch(choice) {
         case 1:
         enter_params();
         break;
         case 2:
         read_cache();
         break;
         case 3:
         write_cache();
         break;
      }
   } while(choice != 4);
   //main memory deallocation
   if(mm.data) {
      freeMainMemory(&mm);
   }
   //cache memory deallocation
   if(cache) {
      free_cache(cache);
   }
   
   printf("*** Memory Freed Up - Program Terminated Normally");
   return 0;
}
