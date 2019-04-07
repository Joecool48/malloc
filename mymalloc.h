#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<unistd.h>

void allocateIfNeeded(size_t size);


void* allocateNewBlock(size_t size);


uint8_t checkCanUse(uint8_t * current, size_t size);


uint8_t splitBlockIfNeeded(uint8_t * current, size_t size);


uint8_t isFree(uint8_t * current);


size_t getSize(uint8_t * current);


uint8_t mergeBlocks(uint32_t posFromBeginning, uint32_t numBlocks);


void * checkForAvailBlock(size_t size);


void* mymalloc(size_t size);

void myfree(void * ptr);
