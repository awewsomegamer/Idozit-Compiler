#include <design1.h>
#include <lexer.h>
#include <parser.h>
#include <semantics.h>
#include <messages.h>
#include <generator.h>

#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* context_t expression(const char*, ...) :
 * Will take the given string and return it in a
 * context_t. This context_t will contain a list
 * of the variables and will also contain a pointer
 * to the head node of the generated tree code.
 * 
 * const char* : The expression to be converted into tree code
 * ...         : The variables (order will dictate calling order)
 *                             ( expression("x + y", "x", "y") will
 *                               will result int the generated function
 *                               being called as func(x, y);            )
 */
context_t expression(const char *form, char **variables)
{
        expression_string = form;
        variables_list = variables;
        variable_count = sizeof(variables) / sizeof(variables[0]);
        expression_ptr = 0;
        
        tree_code_t* tree = build_tree();
        validate(tree);

        context_t* ctx = malloc(sizeof(context_t));
        ctx->head = tree;

        return *ctx;
}

code_block_t compile(context_t context) {
        return default_x86_64_generator(context.head);
}

double run(code_block_t code, double *variables) {
        void* buf;
	buf = mmap(0, code.code_size + code.data_size, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memcpy(buf, code.func, code.code_size);
        memcpy(buf + code.code_size, code.data, code.data_size);

        // for (int i = 0; i < code.code_size + code.data_size; i++)
        //         printf("%02X ", *(((uint8_t*)buf) + i));
        // printf("\n");
        
        for (int i = sizeof(variables) / sizeof(variables[0]) - 1; i >= 0 ; i--) {
                for (int i = 0; i < 8; i++)
                        printf("%02X ", *((uint8_t*)&variables[i] + i));
                printf("\n");

                asm("push %0" : : "a"(variables[i]) :);
        }

        asm("push rcx;    \
             mov rcx, %0;" : : "a"((uintptr_t)buf + code.code_size) :);

        uint64_t rsp = 0;
        asm("mov %0, rsp" : "=a"(rsp) : :);
        for (int i = 0; i < 0x20; i++)
                printf("%02X ", *((uint8_t*)rsp + i));
        printf("\n");

        double result = ((double (*) (void))buf)();

        asm("pop rcx");

        return result;
}

/* void set_error_handler(void*) :
 * This function will set the error handler
 * to the given void* (function).
 */
void set_message_handler(void (*func)(int, const char *, va_list))
{
       _set_message_handler(func); 
}

/* void default_error_handler() :
 * This function will revert the error handler
 * to the default error handler. 
 */
void default_message_handler()
{
        _set_message_handler(NULL);
}