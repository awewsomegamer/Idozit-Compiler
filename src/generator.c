#include <generator.h>
#include <lexer.h>

uint8_t* buffer = NULL;
int position = 0;

#define append_byte(b) \
        buffer = realloc(buffer, ++position); \
        *(buffer + position - 1) = b;

int reg = 0;

int evaluate(tree_code_t *tree) {
        int left = 0, right = 0;

        if (tree->left != NULL) left = evaluate(tree->left);
        if (tree->right != NULL) right = evaluate(tree->right);

        switch (tree->type) {
        case T_NUMBER:
                append_byte(0x48);
                append_byte(0xC7);
                append_byte((0xC0 + reg));
                
                for (int i = 0; i < 4; i++) {
                        int value = (int)tree->value;
                        append_byte(((value >> (8 * i)) & 0xFF));
                }
                
                reg++;

                return reg - 1;

        case T_ADD:
                append_byte(0x48);
                append_byte(0x01);
                append_byte((0xC0 + left + (right * 8)));  

                return left;
        }

        return 0;
}

code_block_t default_x86_64_generator(tree_code_t *tree) {
        code_block_t* ret = malloc(sizeof(code_block_t));
        buffer = malloc(1);
        position = 0;

        evaluate(tree);
        append_byte(0xC3);

        for (int i = 0; i < position; i++)
                printf("%02X ", *(buffer + i));
        printf("\n");

        ret->func = buffer;
        ret->size = position;

        return *ret;
}