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
int spill = 0;

int allocate_reg()
{
        for (int i = 0; i < 16; i++)
                if (xmm_reg[i] != 1) {
                        xmm_reg[i] = 1;
                        return i;
                }

        // Register spill
        int reg = (spill++ % 16) * 8;

        // MOVQ RAX, XMMR
        append_byte(0x66);
        append_byte(0x48);
        append_byte(0x0F)
        append_byte(0x7E);
        append_byte(0xC0 + reg);

        // PUSH RAX
        append_byte(0x55);

        return reg;
}

void free_reg(int reg)
{
        if (spill > 0) {
                // Register unspill
                
                int reg = ((--spill) % 16) * 8;
                // POP RAX
                append_byte(0x58);

                // MOVQ XMMR, RAX
                append_byte(0x66);
                append_byte(0x48);
                append_byte(0x0F)
                append_byte(0x7E);
                append_byte(0xC0 + reg);
        } else {
                xmm_reg[reg] = 0;
        }
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
double numerical_evaluation(tree_code_t* tree, uint8_t* flags)
{
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

/* int evaluate(tree_code_t *tree) :
 * Takes the given tree and generates it into x86-64
 * machine code into the buffer
 */
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

        // Generate to the left
        if (tree->left != NULL) {
                left = evaluate(tree->left);
                vleft = numerical_evaluation(tree->left, &left_info);
        }

        // Generate to the right
        if (tree->right != NULL) {
                right = evaluate(tree->right);
                vright = numerical_evaluation(tree->right, &right_info);
        }

        switch (tree->type) {
        case T_INT: {
                INTEGER_CONSTANT_GENERATION:
                if ((int)tree->value != 0) {
                        // MOV RAX, dword
                        append_byte(0x48);
                        append_byte(0xC7);
                        append_byte(0xC0);
        
                        for (int i = 0; i < 4; i++) {
                                int value = (int)tree->value;
                                append_byte(((value >> (8 * i)) & 0xFF));
                        }
                } else {
                        // XOR RAX, RAX
                        append_byte(0x48);
                        append_byte(0x31);
                        append_byte(0xC0);
                }

                // CVTSI2SD XMMR, RAX
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
                // MOVSD XMMR, [RCX +  N] ; N is dword marked as a reference (RCX is used as a base address for the data buffer) 
                append_byte(0xF2);
                append_byte(0x0F);

                int reg = allocate_reg();

                if (left >= 8) {
                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                }

                append_byte(0x10);
                append_byte(0x04 + ((reg % 8) * 8));
                append_byte(0x0D);
                insert_reference(tree->value, position);

                for(int i = 0; i < 4; i++) {
                        append_byte(0x00);
                }

                return reg;
        }

        case T_VAR: {
                // MOVSD XMMR, [RBP + (V * 0x8)] ; V is the index of the variable in list provided in expression function
                int reg = allocate_reg();

                append_byte(0xF2);
                
                if (left >= 8) {
                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                }

                append_byte(0x0F);
                append_byte(0x10);
                append_byte(0x85 + ((reg % 8) * 8));
                
                uint32_t offset = ((uint32_t)(tree->value + 1) * 8 + 0x8);
                
                for (int i = 0; i < 4; i++) {
                        append_byte(((offset) >> (8 * i)) & 0xFF);
                }

                return reg;
        }

        case T_ADD:
                // ADDSD left, right
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                }

                append_byte(0x0F);
                append_byte(0x58);
                append_byte(0xC0 + right + ((left % 8) * 8));  

                free_reg(right);

                return left;

        case T_SUB:
                // SUBSD left, right
                append_byte(0xF2);
                
                if (left >= 8) {
                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                }

                append_byte(0x0F);
                append_byte(0x5C);
                append_byte(0xC0 + right + ((left % 8) * 8));  

                free_reg(right);

                return left;

        case T_MUL:
                // MULSD left, right
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                }

                append_byte(0x0F);
                append_byte(0x59);
                append_byte(0xC0 + right + ((left % 8) * 8));

                free_reg(right);

                return left;

        case T_DIV:
                // DIVSD left, right
                append_byte(0xF2);

                if (left >= 8) {
                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                }

                append_byte(0x0F);
                append_byte(0x5E);
                append_byte(0xC0 + right + ((left % 8) * 8));

                free_reg(right);

                return left;

        case T_EXPONENT: {
                if (right_info & 1) {
                        int reg = allocate_reg();

                        // MOV RAX, DOUBLE(1)
                        append_byte(0x48);
                        append_byte(0xB8);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0xF0); // 80
                        append_byte(0x3F); // 3F

                        // MOVQ XMMR, RAX
                        append_byte(0x66);
                        append_byte(0x48 + (reg > 7) * 4);
                        append_byte(0x0F);
                        append_byte(0x6E);
                        append_byte(0xC0 + ((reg % 8) * 8));
                        
                        // CVTSD2SI R8, right
                        append_byte(0xF2);
                        append_byte(0x4C + (right > 7))
                        append_byte(0x0F);
                        append_byte(0x2D);
                        append_byte(0xC0 + (right % 8));

                        // CMP R8, 0
                        append_byte(0x49);
                        append_byte(0x83);
                        append_byte(0xF8);
                        append_byte(0x00);

                        // JL DIV
                        append_byte(0x0F);
                        append_byte(0x8C);
                        // Add in the offset of where to jump to
                        append_byte(0x19 + (left >= 8 || right >= 8 || reg >= 8) * 2);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0x00);

                        // JE END
                        append_byte(0x0F);
                        append_byte(0x84);
                        // Add in the offset of where to jump to
                        append_byte(0x21 + (left >= 8 || right >= 8 || reg >= 8) * 2);
                        append_byte(0x00);
                        append_byte(0x00);
                        append_byte(0x00);

                        // Positive Case (Multiplication)
                        // MULSD XMMR, REG
                        append_byte(0xF2); // 9?

                        if (left >= 8 || right >= 8 || reg >= 8) {
                                append_byte(0x40 + (right >= 8 || reg >= 8) + ((left >= 8) * 4)); // 8?
                        }

                        append_byte(0x0F); // 7
                        append_byte(0x59); // 6
                        append_byte(0xC0 + left + ((reg % 8) * 8)); // 5

                        // SUB R8, 1
                        append_byte(0x49); // 4
                        append_byte(0x83); // 3
                        append_byte(0xE8); // 2
                        append_byte(0x01); // 1

                        // JNZ
                        append_byte(0x0F); // 0
                        append_byte(0x85);
                        append_byte(0xFF - 8 - (right >= 8 || reg >= 8 || left >= 8) - 5);
                        append_byte(0xFF);
                        append_byte(0xFF);
                        append_byte(0xFF);

                        // JMP END
                        append_byte(0xE9); // 0
                        append_byte(0x0E + (left >= 8 || right >= 8 || reg >= 8)); // 1
                        append_byte(0x00); // 2
                        append_byte(0x00); // 3
                        append_byte(0x00); // 4


                        // Negative Case (Division)
                        // DIVSD XMMR, REG
                        append_byte(0xF2); // 9? // 5?

                        if (left >= 8 || right >= 8 || reg >= 8) {
                                append_byte(0x40 + (right >= 8 || reg >= 8) + ((left >= 8) * 4)); // 8? // 6?
                        }

                        append_byte(0x0F); // 7 // 7
                        append_byte(0x5E); // 6 // 8
                        append_byte(0xC0 + left + ((reg % 8) * 8)); // 5 // 9

                        // ADD R8, 1
                        append_byte(0x49); // 4 // 10
                        append_byte(0x83); // 3 // 11
                        append_byte(0xC0); // 2 // 12
                        append_byte(0x01); // 1 // 13

                        // JNZ
                        append_byte(0x0F); // 0 // 14
                        append_byte(0x85); // 15
                        append_byte(0xFF - 8 - (right >= 8 || reg >= 8 || left >= 8) - 5); // 16
                        append_byte(0xFF); // 17
                        append_byte(0xFF); // 18
                        append_byte(0xFF); // 19

                        // End Case
                
                        // MOV left, reg
                        append_byte(0xF2);

                        if (left >= 8 || reg >= 8) {
                                append_byte(0x40 + (reg >= 8) + ((left >= 8) * 4));
                        }

                        append_byte(0x0F);
                        append_byte(0x10);
                        append_byte(0xC0 + reg + ((left % 8) * 8));

                        free_reg(right);
                        free_reg(reg);

                        return left;
                } else {
                        int repetitions = vright - 1;
                        int reg = 0;

                        if (repetitions >= 2) {
                                reg = allocate_reg();

                                append_byte(0xF2);

                                if (left >= 8) {
                                        append_byte(0x40 + (right >= 8) + ((left >= 8) * 4));
                                }

                                append_byte(0x0F);
                                append_byte(0x10);
                                append_byte(0xC0 + left + ((reg % 8) * 8));  
                        }

                        if (repetitions > 4) {
                                // MOV R8, repetitions
                                append_byte(0x49);
                                append_byte(0xC7);
                                append_byte(0xC0);
                                for (int i = 0; i < 4; i++) {
                                        append_byte(((repetitions >> (8 * i)) & 0xFF));
                                }
                                // MULSD XMMR, REG
                                append_byte(0xF2); // 8

                                if (left >= 8 || right >= 8 || reg >= 8) {
                                        append_byte(0x40 + (right >= 8 || reg >= 8) + ((left >= 8) * 4)); // 7?
                                }

                                append_byte(0x0F); // 6
                                append_byte(0x59); // 5
                                append_byte(0xC0 + reg + ((left % 8) * 8)); // 4

                                // DEC R8
                                append_byte(0x49); // 3
                                append_byte(0xFF); // 2
                                append_byte(0xC8); // 1

                                // JNZ 
                                append_byte(0x0F); // 0
                                append_byte(0x85);
                                append_byte(0xFF - 7 - (right >= 8 || reg >= 8 || left >= 8) - 5);
                                append_byte(0xFF);
                                append_byte(0xFF);
                                append_byte(0xFF);

                                free_reg(reg);
                                free_reg(right);

                                return left;
                        }

                        for (int i = 0; i < repetitions; i++) {
                                append_byte(0xF2);

                                if (left >= 8 || right >= 8 || reg >= 8) {
                                        append_byte(0x40 + (right >= 8 || reg >= 8) + ((left >= 8) * 4));
                                }

                                append_byte(0x0F);
                                append_byte(0x59);
                                append_byte(0xC0 + ((repetitions >= 2) ? (reg + ((left % 8) * 8)) : (left + ((left % 8) * 8))));
                        }

                        if (repetitions >= 2)
                                free_reg(reg);    

                        free_reg(right);
                }


                return left;
        }

        }

        return 0;
}

/* void fill_references() :
 * This will fill in the references left 0 within the program
 */
void fill_references()
{
        struct reference* present_references = malloc(sizeof(struct reference));
        int present_reference_count = 0;

        for (int i = 0; i < reference_count; i++) {
                // Check if the reference has already been inserted into the data buffer
                // if so, just go fill it in with the proper offset
                int present_reference_idx = -1;
                for (int j = 0; j < present_reference_count; j++)
                        if (present_references[j].value == references[i].value) {
                                present_reference_idx = j;
                                goto FILL_REFERENCE;
                        }

                // The reference is not yet present in the data buffer, so add it
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

code_block_t default_x86_64_generator(tree_code_t *tree, int var_count)
{
        code_block_t* ret = malloc(sizeof(code_block_t));
        buffer = malloc(1);
        data_buffer = malloc(1);
        position = 0;
        data_position = 0;

        spill = 0;
        for (int i = 0; i < 16; i++)
                xmm_reg[i] = 0;


        // PUSH RBP
        append_byte(0x55);

        // MOV RBP, RSP
        append_byte(0x48);
        append_byte(0x89);
        append_byte(0xE5);

        // // XOR RAX, RAX
        // append_byte(0x48);
        // append_byte(0x31);
        // append_byte(0xC0);

        // // CVTSI2SD XMM0, RAX
        // append_byte(0xF2);
        // append_byte(0x48);
        // append_byte(0x0F);
        // append_byte(0x2A);
        // append_byte(0xC0);

        // Convert tree to x86-64 machine code        
        evaluate(tree);

        // MOV RSP, RBP
        append_byte(0x48);
        append_byte(0x89);
        append_byte(0xEC);

        // POP RBP
        append_byte(0x5D);

        // RET
        append_byte(0xC3);

        // Fill in the missing references
        fill_references();

        ret->code = buffer;
        ret->data = data_buffer;
        ret->code_size = position;
        ret->data_size = data_position;

        return *ret;
}