#include <generator.h>
#include <lexer.h>
#include <string.h>
#include <messages.h>

uint8_t* buffer = NULL;
uint8_t* data_buffer = NULL;

int position = 0;
int data_position = 0;

#define append_byte(b) \
        buffer = realloc(buffer, ++position); \
        *(buffer + position - 1) = b;

struct reference
{
        double value;
        uint64_t position;
};

struct reference* references;
int reference_count = 0;

void insert_reference(double value, uint64_t position)
{
        references = realloc(references, sizeof(struct reference) * (++reference_count));

        references[reference_count - 1].position = position;
        references[reference_count - 1].value = value;
}

int xmm_reg[16];

int allocate_reg()
{
        for (int i = 0; i < 16; i++)
                if (xmm_reg[i] != 1) {
                        xmm_reg[i] = 1;
                        return i;
                }

        message(MESSAGE_FATAL, "No registers free\n");
                        
}

void free_reg(int reg)
{
        xmm_reg[reg] = 0;
}

int evaluate(tree_code_t *tree)
{
        int left = 0, right = 0;

        if (tree->left != NULL) left = evaluate(tree->left);
        if (tree->right != NULL) right = evaluate(tree->right);

        switch (tree->type) {
        case T_INT: {
                append_byte(0x48);
                append_byte(0xC7);
                append_byte(0xC0);
                
                // if mov reg, 0, use xor reg, reg
                // if mov rax, imm use mov rax op
                for (int i = 0; i < 4; i++) {
                        int value = (int)tree->value;
                        append_byte(((value >> (8 * i)) & 0xFF));
                }
                
                int reg = allocate_reg();

                append_byte(0xF2);
                append_byte(0x48 + ((reg >= 8) * 4));
                append_byte(0x0F);
                append_byte(0x2A);
                append_byte(0xC0 + ((reg % 8) * 8));

                return reg;
        }

        case T_NUMBER: {
                append_byte(0xF2);
                append_byte(0x0F);

                int reg = allocate_reg();

                if (reg >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x10);
                append_byte(0x04 + ((reg % 8) * 8));
                append_byte(0x0D);
                insert_reference(tree->value, position);
                for(int i = 0; i < 4; i++) append_byte(0x00);

                return reg;
        }

        case T_VAR: {
                int reg = allocate_reg();

                append_byte(0xF2);
                
                if (reg >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x10);
                append_byte(0x85 + ((reg % 8) * 8));
                
                uint32_t offset = ((uint32_t)tree->value * 8 + 0x18);
                
                for (int i = 0; i < 4; i++) {
                        append_byte(((offset) >> (8 * i)) & 0xFF);
                }

                return reg;
        }

        case T_ADD:
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x58);
                append_byte(0xC0 + right + ((left % 8) * 8));  

                free_reg(right);

                return left;

        case T_SUB:
                append_byte(0xF2);
                
                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x5C);
                append_byte(0xC0 + right + ((left % 8) * 8));  

                free_reg(right);

                return left;

        case T_MUL:
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x59);
                append_byte(0xC0 + right + ((left % 8) * 8));

                free_reg(right);

                return left;

        case T_DIV:
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x5E);
                append_byte(0xC0 + right + ((left % 8) * 8));

                free_reg(right);

                return left;

        case T_EXPONENT:
                

                return left;
        }

        return 0;
}

void fill_references()
{
        struct reference* present_references = malloc(sizeof(struct reference));
        int present_reference_count = 0;

        for (int i = 0; i < reference_count; i++) {
                int present_reference_idx = -1;
                for (int j = 0; j < present_reference_count; j++)
                        if (present_references[j].value == references[i].value) {
                                present_reference_idx = j;
                                goto FILL_REFERENCE;
                        }

                present_references = realloc(present_references, sizeof(struct reference) * (++present_reference_count));
                present_references[present_reference_count - 1].position = data_position;
                present_references[present_reference_count - 1].value = references[i].value;

                uint64_t bytes_double;
                memcpy(&bytes_double, &references[i].value, sizeof(uint64_t));

                for (int j = 0; j < 8; j++) {
                        data_buffer = realloc(data_buffer, ++data_position);
                        *(data_buffer + data_position - 1) = (bytes_double >> (8 * j)) & 0xFF;
                }

                present_reference_idx = present_reference_count - 1;
                
                FILL_REFERENCE:

                for (int j = 0; j < 4; j++)
                        *(buffer + references[i].position + j) = ((present_references[present_reference_idx].position) >> (8 * j)) & 0xFF;
        }
}

code_block_t default_x86_64_generator(tree_code_t *tree)
{
        code_block_t* ret = malloc(sizeof(code_block_t));
        buffer = malloc(1);
        data_buffer = malloc(1);
        position = 0;
        data_position = 0;

        // PUSH RBP
        append_byte(0x55);

        // MOV RBP, RSP
        append_byte(0x48);
        append_byte(0x89);
        append_byte(0xE5);
        
        evaluate(tree);

        // POP RBP
        append_byte(0x5D);

        // RET
        append_byte(0xC3);

        // Fill in the missing references
        fill_references();

        ret->func = buffer;
        ret->data = data_buffer;
        ret->code_size = position;
        ret->data_size = data_position;

        return *ret;
}