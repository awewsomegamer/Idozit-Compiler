#include <cacher.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

struct cache_block {
        size_t size;
        void *where;
        uint8_t properties;
        code_block_t *code;

        // uint64_t times_used; 
        time_t last_used;
};

size_t blocks_size = 0;
size_t blocks_ptr = 0;
struct cache_block *blocks;
uint64_t next_address = 0;

void *cache_code_block(code_block_t *code) {
        printf("Number of blocks: %d\nCurrent index: %d\n", blocks_size, blocks_ptr);

        // Check if code block is already cached
        for (int i = 0; i < blocks_ptr; i++)
                if (blocks[i].code == code) {
                        printf("Found cached entry (%d), returning\n", i);
                        blocks[i].last_used = time(NULL);
                        return blocks[i].where;
                }

        // Clean cache spac
        // Make sure there are enough blocks
        if (blocks_size != idozit_word.available_caches) {
                printf("Number of caches changed, updating size\n");
                blocks = realloc(blocks, sizeof(struct cache_block) * idozit_word.available_caches);
                blocks_size = idozit_word.available_caches;
        }

        // Simple bubble sort (replace later)
        for (int i = 0; i < blocks_ptr; i++) {
                for (int j = 0; j < blocks_ptr - 1; j++) {
                        if (blocks[j + 1].where > blocks[j].where) {
                                struct cache_block tmp = blocks[j];
                                blocks[j] = blocks[j + 1];
                                blocks[j + 1] = tmp;
                        }
                }
        }

        // Free old caches
        for (int i = 0; i < blocks_ptr; i++)
                if (blocks[i].last_used >= idozit_word.cache_cut_off + time(NULL)) {
                        printf("Cache %d older than designated time, freeing\n", i);
                        blocks[i].properties = 0; // Mark as free
                }

        // Combine free caches

        // Find a space for *code
        //      Esnure it is within user specification

        // Check if there is a free space available to use
        for (int i = 0; i < blocks_ptr; i++)
                if ((blocks[i].size >= code->code_size + code->data_size && (blocks[i].properties & 1) == 0)) {
                        // Use this block
                        memcpy(blocks[i].where, code->code, code->code_size);
                        memcpy(blocks[i].where + code->code_size, code->data, code->data_size);

                        blocks[i].last_used = time(NULL);
                        blocks[i].code = code;

                        printf("Block %d\n is free, returning for use\n", i);

                        return blocks[i].where;
                }


        // If we are maxed out check for oldest / least used block
        if (blocks_ptr == blocks_size) {
                int oldest = 0;
                time_t oldest_time = blocks[0].last_used;
                for (int i = 0; i < blocks_size; i++) {
                        if (blocks[i].last_used < oldest_time) {
                                oldest_time = blocks[i].last_used;
                                oldest = i;
                        }
                        printf("%d: %d : %d\n", i, blocks[i].last_used, oldest_time);
                }
                
                memcpy(blocks[oldest].where, code->code, code->code_size);
                memcpy(blocks[oldest].where + code->code_size, code->data, code->data_size);
                
                blocks[oldest].last_used = time(NULL);
                blocks[oldest].code = code;

                printf("Cache filled, selecting relatively oldest block (%d), returning for use\n", oldest);

                return blocks[oldest].where;
        }
        
        // Allocate a new cache block for the code
        blocks[blocks_ptr].where = mmap((void *)next_address, code->code_size + code->data_size,
                                        PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        next_address += code->code_size + code->data_size;

        memcpy(blocks[blocks_ptr].where, code->code, code->code_size);
        memcpy(blocks[blocks_ptr].where + code->code_size, code->data, code->data_size);

        blocks[blocks_ptr].last_used = time(NULL);
        blocks[blocks_ptr].code = code;

        printf("Miss, allocating new block at %d\n", blocks_ptr);

        return blocks[blocks_ptr++].where;
}