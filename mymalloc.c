#include"mymalloc.h"

static uint8_t* head; // pointer to the first block. NULL if none were allocated (head of linked list)
static uint8_t* tail;
static size_t num_blocks; // the amount of blocks we have
static size_t currentBytesUsed; // current place we are in memory (sbrk)
static uint8_t* startMem; // base address

// the size of each page/block in bytes
#define PAGE_SIZE 4096

typedef struct {
    size_t size;
    uint8_t isFree;
    uint8_t * next;
} header;

size_t ceilSize(float num) {
    if (num > (size_t)num) return (size_t)num + 1;
    else return (size_t) num;
}

// creates a new blocks if needed because the size is too much
void allocateIfNeeded(size_t size) {
    // we already have enough memory
    if(currentBytesUsed + size < num_blocks * PAGE_SIZE) return;
    // otherwise calculate number of blocks to allocate
    size_t amountBlocksToAllocate = ceilSize((size - ((num_blocks * PAGE_SIZE) - currentBytesUsed)) / (float)PAGE_SIZE);
    if (!startMem)
        startMem = (uint8_t*) sbrk((amountBlocksToAllocate * PAGE_SIZE));
    else
        (void) sbrk(amountBlocksToAllocate * PAGE_SIZE);
    num_blocks += amountBlocksToAllocate;
}

// adds a new block to the end of the list by traversing it
// returns a pointer to the new block starting at the header
void* allocateNewBlock(size_t size) {
    if ((!tail && head) || (!head && tail)) printf("Error with linked list\n");
    // create a new block and assign it to head and tail
    allocateIfNeeded(size); // determines if sbrk needs to be called to request another block
    
    uint8_t * currentPtr = startMem + currentBytesUsed;
    header newHeader;
    newHeader.size = size;
    newHeader.isFree = 1;
    newHeader.next = NULL;
    memcpy(currentPtr, &newHeader, sizeof(header)); // copy the header to its position

    // increment bytes used
    currentBytesUsed += sizeof(header) + size;
    
    // if head isnt assigned then assign it, otherwise append to tail
    if (!head) {
        head = currentPtr;
        tail = currentPtr;
    }
    else {
        ((header*)tail)->next = currentPtr;
        tail = currentPtr;
    }
    return currentPtr;

}

// returns 1 if usable and 0 otherwise
uint8_t checkCanUse(uint8_t * current, size_t size) {
    header* structPtr = (header*)current;
    return (structPtr->size >= size && structPtr->isFree);
}


// takes in a block pointer and a needed size, and cuts the block if is greater than
// twice the size necissary
uint8_t splitBlockIfNeeded(uint8_t * current, size_t size) {
    header * structPtr = (header*) current;
    if (structPtr->size / 2 >= size - sizeof(header)) {
        size_t newSize = structPtr->size - size - sizeof(header);
        // allocate a new block within this block, and then add it to the linked list
        uint8_t * newBlockStart = current + sizeof(header) + size;
        header * newHeader= (header*) newBlockStart;

        uint8_t * nextHeader = structPtr->next;
        structPtr->next = nextHeader; // assign the next pointer to this header
        newHeader->size = newSize;
        newHeader->isFree = 1;
        newHeader->next = nextHeader;
        return 1;
    }
    return 0;
}


// returns 1 if the block is free, and 0 if it isn't
uint8_t isFree(uint8_t * current) {
    header * newPtr = (header*) current;
    return newPtr->isFree;
}

size_t getSize(uint8_t * current) {
    header * newPtr = (header*) current;
    return newPtr->size;
}

uint8_t mergeBlocks(uint32_t posFromBeginning, uint32_t numBlocks) {
    // iterate to that position
    uint8_t * current = head;
    uint32_t currentPos = 0;
    while (current && currentPos < posFromBeginning) {
        current = ((header*)current)->next;
        currentPos++;
    }
    if (currentPos != posFromBeginning) printf("Something wrong with mergeBlocks\n");
    // reached start point now go to end and add up the size
    uint8_t * startAnchor = current;
    uint32_t blockCounter = 0;
    uint32_t totalSize = 0;
    // count through all the blocks
    while(blockCounter < numBlocks) {
        totalSize += ((header*)current)->size + sizeof(header); // add size
        blockCounter++;
        current = ((header*)current)->next;
    }
    uint8_t * endAnchor = ((header*)current)->next;
    ((header*)startAnchor)->next = endAnchor;
    ((header*)startAnchor)->size = totalSize - sizeof(header);
    return 1;
}

void * checkForAvailBlock(size_t size) {
    uint8_t * current = head;
    uint32_t freeBlockCount = 0;
    uint32_t freeBlocksTotalSize = 0;
    uint32_t blockPos = 0;
    while(current) {
        // found a block that is the right size
        if (checkCanUse(current, size)) {
            // to reduce fragmentation internally we should split the block if it is too big
            (void) splitBlockIfNeeded(current, size);
            if (((header*)current)->size < size) printf("Malloc allocation error\n");
            return current + sizeof(header);
        }
        else if (isFree(current)) {
            freeBlockCount += 1;
            freeBlocksTotalSize += getSize(current);
            if (freeBlocksTotalSize >= size) {
                // we now have enough blocks to merge to get a large enough sized block
                mergeBlocks(blockPos - freeBlockCount, freeBlockCount); // merge those blocks to create a large enough one
            }
        }
        else {
            freeBlockCount = 0;
            freeBlocksTotalSize = 0;
        }
        blockPos++; // keep track of position in linked list for merging
        current = ((header*)current)->next; 
    }
}

void* mymalloc(size_t size) {
    if (!size) return NULL;
    if (!head) 
        return (void*)((uint8_t*)allocateNewBlock(size) + sizeof(header));
    // first we must check for available blocks
    void * avail_block = checkForAvailBlock(size); // see if a block exists thats big enough for us
    
    // if one isnt big enough
    if (!avail_block) 
        avail_block = allocateNewBlock(size);

    return avail_block + sizeof(header);
    
}

// free call
void myfree(void * ptr) {
    // traverse the list and find which one to free
    uint8_t * current = ptr;
    current -= sizeof(header);
    header * freeHeader = (header*) current;
    freeHeader->isFree = 1; // simply free the block
}
