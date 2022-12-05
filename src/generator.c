#include <generator.h>
#include <lexer.h>
#include <string.h>
#include <messages.h>

uint8_t* buffer = NULL;
uint8_t* data_buffer = NULL;

int position = 0;
int code_size = 0;
int data_position = 0;

#define append_byte(b)                        \
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

// Flags:
// Bit 0: Set = Cannot calculate (variable)
// Bit 1: Set = Return should be seen as an integer
// Bit 2:
// Bit 3:
// Bit 4:
// Bit 5:
// Bit 6:
// Bit 7:
double numerical_evaluation(tree_code_t* tree, uint8_t* flags) {
        double left = 0, right = 0;
        if (tree->left != NULL) left = numerical_evaluation(tree->left, flags);
        if (tree->right != NULL) right = numerical_evaluation(tree->right, flags);

        // Return if variable is found
        if ((*flags & 1) == 1)
                return 0;

        switch (tree->type) {
        case T_NUMBER:
                *flags |= 1 << 1;
        case T_INT:
                return tree->value;

        case T_VAR:
                *flags |= 1;
                return tree->value;

        case T_ADD:
                return left + right;

        case T_SUB:
                return left - right;

        case T_MUL:
                return left * right;

        case T_DIV: 
                return left / right;

        case T_EXPONENT: {
                double res = 1;
                
                if (right >= 0)
                        for (int i = 0; i < right; i++)
                                res *= left;
                else
                        for (int i = 0; i < right; i++)
                                res /= left;

                return res;
        }
        }
}

int evaluate(tree_code_t *tree)
{       
        uint8_t flags = 0;
        double solution = numerical_evaluation(tree, &flags);

        if ((flags & 1) == 0) {
                // Found numeric solution

                tree->value = solution;

                if (((flags >> 1) & 1) == 0) {
                        // Solution is an integer
                        goto INTEGER_CONSTANT_GENERATION;
                } else {
                        // Solution is a double
                        goto DOUBLE_CONSTANT_GENERATION;
                }
        }

        // Did not find a numeric solution

        int left = 0, right = 0;
        double vleft = 0, vright = 0;
        uint8_t left_info = 0, right_info = 0;

        int start_pointer = position;

        if (tree->left != NULL) {
                left = evaluate(tree->left);
                vleft = numerical_evaluation(tree->left, &left_info);
        }

        int left_add_pointer = position;

        if (tree->right != NULL){
                right = evaluate(tree->right);
                vright = numerical_evaluation(tree->right, &right_info);
        }

        int right_add_pointer = position;


        // // Calculate the difference, error, between the code generated for the
        // // left branch and code generated for the right branch
        // int lr_error = 0;
        // if (left_add_pointer - start_pointer == right_add_pointer - left_add_pointer)
        //         for (int i = start_pointer; i < left_add_pointer - start_pointer; i++)
        //                 if (*(buffer + i) != *(buffer + left_add_pointer + i))
        //                         lr_error++;
        
        // // If the left branch and right branch values are the same, or the error
        // // between them is below or 1 byte, then remove right branch and use the
        // // left branch
        // if (vleft == vright) {
        //         printf("%d %d\n", position, left_add_pointer);

        //         uint8_t* new_buffer = malloc(left_add_pointer - 1);
        //         memcpy(new_buffer, buffer, left_add_pointer - 1);
        //         for (int i = 0; i < left_add_pointer; i++)
        //                 printf("%02X ", *(new_buffer + i));

        //         printf("\n");

        //         for (int i = 0; i < position; i++)
        //                 printf("%02X ", *(buffer + i));

        //         printf("\n");

        //         // free(buffer);
        //         free_reg(right);
        //         right = left;
        //         // buffer = new_buffer;
        //         // position = left_add_pointer;
        // }

        switch (tree->type) {
        case T_INT: {
                INTEGER_CONSTANT_GENERATION:
                if ((int)tree->value != 0) {
                        append_byte(0x48);
                        append_byte(0xC7);
                        append_byte(0xC0);
        
                        for (int i = 0; i < 4; i++) {
                                int value = (int)tree->value;
                                append_byte(((value >> (8 * i)) & 0xFF));
                        }
                } else {
                        append_byte(0x48);
                        append_byte(0x31);
                        append_byte(0xC0);
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
                DOUBLE_CONSTANT_GENERATION:
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

                if (left != right) free_reg(right);

                return left;

        case T_SUB:
                append_byte(0xF2);
                
                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x5C);
                append_byte(0xC0 + right + ((left % 8) * 8));  

                if (left != right) free_reg(right);

                return left;

        case T_MUL:
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x59);
                append_byte(0xC0 + right + ((left % 8) * 8));

                if (left != right) free_reg(right);

                return left;

        case T_DIV:
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x44);
                }

                append_byte(0x0F);
                append_byte(0x5E);
                append_byte(0xC0 + right + ((left % 8) * 8));

                if (left != right) free_reg(right);

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

        uint8_t f = 0;
        double a = numerical_evaluation(tree, &f);
        printf("%f %X\n", a, f);

        ret->func = buffer;
        ret->data = data_buffer;
        ret->code_size = position;
        ret->data_size = data_position;

        return *ret;
}