#include <cacher.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <messages.h>

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

time_t get_time()
{
        struct timeval now;
        gettimeofday(&now, NULL);
        return 1000000 * now.tv_sec + now.tv_usec; // Microseconds
}

void *miss(code_block_t *code, int *counter)
{
        // Allocate a new cache block for the code
        blocks[*counter].where = mmap((void *)next_address, code->code_size + code->data_size,
                                        PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        next_address += code->code_size + code->data_size;

        memcpy(blocks[*counter].where, code->code, code->code_size);
        memcpy(blocks[*counter].where + code->code_size, code->data, code->data_size);

        blocks[*counter].last_used = get_time();
        blocks[*counter].code = code;
        blocks[*counter].size = code->code_size + code->data_size;
        blocks[*counter].properties |= 1;

        message(MESSAGE_DEBUG, "Miss, allocating new block at %d\n", *counter);

        return blocks[(*counter)++].where;
}

void *cache_code_block(code_block_t *code) {
        message(MESSAGE_DEBUG, "Number of blocks: %d\nCurrent index: %d\n", blocks_size, blocks_ptr);

        // Check if code block is already cached
        for (int i = 0; i < blocks_ptr; i++)
                if (blocks[i].code == code) {
                        message(MESSAGE_DEBUG, "Found cached entry (%d), returning\n", i);
                        blocks[i].last_used = get_time();
                        return blocks[i].where;
                }

        // Clean cache spac
        // Make sure there are enough blocks
        if (blocks_size != idozit_word.available_caches) {
                message(MESSAGE_DEBUG, "Number of caches changed, updating size\n");
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
                if (blocks[i].last_used >= idozit_word.cache_cut_off + get_time()) {
                        message(MESSAGE_DEBUG, "Cache %d older than designated time, freeing\n", i);
                        blocks[i].properties = 0; // Mark as free
                }

        // Find a space for *code
        //      Esnure it is within user specification

        // Check if there is a free space available to use
        for (int i = 0; i < blocks_ptr; i++)
                if ((blocks[i].size >= code->code_size + code->data_size && (blocks[i].properties & 1) == 0)) {
                        // Use this block
                        memcpy(blocks[i].where, code->code, code->code_size);
                        memcpy(blocks[i].where + code->code_size, code->data, code->data_size);

                        blocks[i].last_used = get_time();
                        blocks[i].code = code;

                        message(MESSAGE_DEBUG, "Block %d is free, returning for use\n", i);

                        return blocks[i].where;
                } else if ((blocks[i].properties & 1) == 0) {
                        // Make a bigger block and replace this one
                        message(MESSAGE_DEBUG, "Block %d is free, too small\n", i);
                        munmap(blocks[i].where, blocks[i].size);
                        return miss(code, &i);
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
                }
                // Make sure current entry is big enough
                if (blocks[oldest].size >= code->code_size + code->data_size) {
                        memcpy(blocks[oldest].where, code->code, code->code_size);
                        memcpy(blocks[oldest].where + code->code_size, code->data, code->data_size);
                        
                        blocks[oldest].last_used = get_time();
                        blocks[oldest].code = code;
                        blocks[oldest].size = code->code_size + code->data_size;

                        message(MESSAGE_DEBUG, "Cache filled, selecting relatively oldest block (%d), returning for use\n", oldest);

                        return blocks[oldest].where;
                } else {
                        // Free the last buffer and replace it with a bigger buffer
                        message(MESSAGE_DEBUG, "Relatively oldest buffer (%d) found, too small\n");
                        munmap(blocks[oldest].where, blocks[oldest].size);
                        return miss(code, &oldest);
                }
        }

        return miss(code, &blocks_ptr);
}