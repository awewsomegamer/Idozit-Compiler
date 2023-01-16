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
        // Check if code block is already cached
        for (int i = 0; i < blocks_ptr; i++)
                if (blocks[i].code == code) {
                        blocks[i].last_used = time(NULL);
                        return blocks[i].where;
                }

        // Clean cache spac
        // Make sure there are enough blocks
        if (blocks_size != idozit_word.available_caches) {
                blocks = realloc(blocks, sizeof(struct cache_block) * idozit_word.available_caches);
                blocks_size = idozit_word.available_caches;
        }

        // Free old caches
        for (int i = 0; i < blocks_ptr; i++)
                if (blocks[i].last_used >= idozit_word.cache_cut_off)
                        blocks[i].properties = 0; // Mark as free

        // Combine free caches

        // Find a space for *code
        //      Esnure it is within user specification

        // Check if there is a free space available to use
        for (int i = 0; i < blocks_ptr; i++)
                if (blocks[i].size >= code->code_size + code->data_size 
                    && (blocks[i].properties & 1) == 0) {
                        // Use this block
                        memcpy(blocks[i].where, code->code, code->code_size);
                        memcpy(blocks[i].where + code->code_size, code->data, code->data_size);
                        
                        blocks[i].last_used = time(NULL);

                        return blocks[i].where;
                    }


        // Allocate a new cache block for the code
        blocks[blocks_ptr].where = mmap((void *)next_address, code->code_size + code->data_size,
                                        PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        memcpy(blocks[blocks_ptr].where, code->code, code->code_size);
        memcpy(blocks[blocks_ptr].where + code->code_size, code->data, code->data_size);

        blocks[blocks_ptr].last_used = time(NULL);

        return blocks[blocks_ptr++].where;
}