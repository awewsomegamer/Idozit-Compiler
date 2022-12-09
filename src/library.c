#include <design1.h>
#include <lexer.h>
#include <parser.h>
#include <semantics.h>
#include <messages.h>
#include <generator.h>

#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// Custom code generator function pointer
code_block_t (*code_generator)(tree_code_t *) = NULL;

context_t expression(const char *form, int var_count, ...)
{
        expression_string = form;
        variable_count = var_count;
        expression_ptr = 0;
        
        va_list args;
        va_start(args, var_count);
        variables_list = malloc(sizeof(char *) * var_count);

        for (int i = var_count - 1; i >= 0; i--) {
                char *var = va_arg(args, char *);
                variables_list[i] = malloc(strlen(var));
                strcpy(&(*variables_list[i]), var);
        }

        tree_code_t* tree = build_tree();
        validate(tree);

        context_t *ctx = malloc(sizeof(context_t));
        ctx->head = tree;
        ctx->var_count = var_count;

        for (int i = 0; i < var_count; i++)
                free(variables_list[i]);
        free(variables_list);

        return *ctx;
}

code_block_t compile(context_t context)
{
        code_block_t ret;

        if (code_generator == NULL)
                ret = default_x86_64_generator(context.head);
        else
                ret = (*code_generator)(context.head);

        
        ret.var_count = context.var_count;

        return ret;
}

double run(code_block_t code, ...)
{
        void *buf;
	buf = mmap(0, code.code_size + code.data_size, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memcpy(buf, code.func, code.code_size);
        memcpy(buf + code.code_size, code.data, code.data_size);

        // Print machine code + data
        char* machine_code_string = malloc((code.code_size + code.data_size) * 3);
        for (int i = 0; i < code.code_size + code.data_size; i++)
                sprintf(machine_code_string + i * 3, "%02X ", *(((uint8_t *)buf) + i));
        message(MESSAGE_DEBUG, "%s\n", machine_code_string);

        va_list args;
        va_start(args, code);

        for (int i = 0; i < code.var_count; i++) {
                uint64_t bytes_double;
                double arg = va_arg(args, double);
                memcpy(&bytes_double, &arg, sizeof(uint64_t));

                asm("push %0" : : "a"(bytes_double) :);
        }

        asm("push rcx;    \
             mov rcx, %0;" : : "a"((uintptr_t)buf + code.code_size) :);

        double result = ((double (*) (void))buf)();

        asm("pop rcx");

        munmap(buf, code.code_size + code.data_size);

        return result;
}

void set_message_handler(void (*func)(int, const char *, va_list))
{
       _set_message_handler(func); 
}

void default_message_handler()
{
        _set_message_handler(NULL);
}

void set_code_generator(code_block_t (*generator)(tree_code_t *))
{
        code_generator = generator;
}

void default_code_generator()
{
        code_generator = NULL;
}