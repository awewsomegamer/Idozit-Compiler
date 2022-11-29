#include <generator.h>
#include <lexer.h>

uint8_t* buffer = NULL;
uint8_t* data_buffer = NULL;

int position = 0;
int data_position = 0;

#define append_byte(b) \
        buffer = realloc(buffer, ++position); \
        *(buffer + position - 1) = b;

int reg = 0;
int xmm_reg = 0;

int evaluate(tree_code_t *tree) {
        int left = 0, right = 0;

        if (tree->left != NULL) left = evaluate(tree->left);
        if (tree->right != NULL) right = evaluate(tree->right);

        switch (tree->type) {
        case T_INT:
                append_byte(0x48);
                append_byte(0xC7);
                append_byte((0xC0 + reg));
                
                // if mov reg, 0, use xor reg, reg
                // if mov rax, imm use mov rax op
                for (int i = 0; i < 4; i++) {
                        int value = (int)tree->value;
                        append_byte(((value >> (8 * i)) & 0xFF));
                }
                
                append_byte(0xF2);
                append_byte(0x48); 
                append_byte(0x0F);
                append_byte(0x2A);
                append_byte(0xC0 + (xmm_reg * 8));
                xmm_reg++;

                return xmm_reg - 1;

        case T_NUMBER:
                

                break;

        case T_ADD:
                // append_byte(0x48);
                // append_byte(0x01);
                // append_byte((0xC0 + left + (right * 8)));

                append_byte(0xF2);
                append_byte(0x0F);
                append_byte(0x58);
                append_byte(0xC0 + right + (left * 8));  

                return left;
        }

        return 0;
}

void fill_references() {

}

code_block_t default_x86_64_generator(tree_code_t *tree) {
        code_block_t* ret = malloc(sizeof(code_block_t));
        buffer = malloc(1);
        data_buffer = malloc(1);
        position = 0;
        data_position = 0;

        evaluate(tree);
        append_byte(0xC3);
        fill_references();

        if (data_position > 0) {
                buffer = realloc(buffer, position + data_position);
                for (int i = position; i < position + data_position; i++)
                        *(buffer + i) = *(data_buffer + (i - position));
        }

        for (int i = 0; i < position + data_position; i++)
                printf("%02X ", *(buffer + i));
        printf("\n");

        ret->func = buffer;
        ret->size = position;

        free(data_buffer);

        return *ret;
}