#include <idozit.h>
#include <lexer.h>
#include <parser.h>
#include <semantics.h>
#include <messages.h>
#include <generator.h>

#include <cacher.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

// Custom code generator function pointer
code_block_t (*code_generator)(tree_code_t *, int) = NULL;
double (*run_func)(code_block_t *, va_list) = NULL;

code_block_t *last_exec = NULL;

pthread_mutex_t expression_mutex;
pthread_mutex_t compile_mutex;

// This one is debatable
pthread_mutex_t run_mutex;

struct idozit_word_struct idozit_word;


context_t expression(const char *form, int var_count, ...)
{
        pthread_mutex_lock(&expression_mutex);

        // Setup inputs
        expression_string = form;
        variable_count = var_count;
        expression_ptr = 0;
        
        va_list args;
        va_start(args, var_count);

        // Fill list that contains variable names
        variables_list = malloc(sizeof(char *) * var_count);
        for (int i = var_count - 1; i >= 0; i--) {
                char *var = va_arg(args, char *);
                variables_list[i] = malloc(strlen(var));
                strcpy(&(*variables_list[i]), var);
        }

        // Get valid tree
        tree_code_t *tree = build_tree();
        validate(tree);

        // Build context
        context_t *ctx = malloc(sizeof(context_t));
        ctx->head = tree;
        ctx->var_count = var_count;

        // Free variables list
        for (int i = 0; i < var_count; i++)
                free(variables_list[i]);
        free(variables_list);

        pthread_mutex_unlock(&expression_mutex);

        return *ctx;
}

code_block_t compile(context_t context)
{
        pthread_mutex_lock(&compile_mutex);

        code_block_t ret;

        // Generate code, use custom generator if it is not NULL otherwise use default one
        if (code_generator == NULL)
                ret = default_x86_64_generator(context.head, context.var_count);
        else
                ret = (*code_generator)(context.head, context.var_count);


        ret.var_count = context.var_count;

        pthread_mutex_unlock(&compile_mutex);

        return ret;
}

double run(code_block_t *code, ...)
{      
        // pthread_mutex_lock(&run_mutex);

        va_list args;
        va_start(args, code);
        if (run_func != NULL)
                return (*run_func)(code, args);

        void *func = code->func;
        // Create a new executable buffer if we haven't ran anything or
        // running a different function.
        if ((last_exec == NULL || last_exec != code) && !idozit_word.caching) {
                if (last_exec != NULL)
                        munmap(0, last_exec->code_size + last_exec->data_size);

                func = code->func = mmap(0, code->code_size + code->data_size, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                memcpy(code->func, code->code, code->code_size);
                memcpy(code->func + code->code_size, code->data, code->data_size);

                last_exec = code;
        } else if (idozit_word.caching) {
                func = cache_code_block(code);
        }
        
        // Print machine code + data
        #ifdef DEBUG
                char* machine_code_string = malloc((code->code_size + code->data_size) * 3);
                for (int i = 0; i < code->code_size + code->data_size; i++)
                        sprintf(machine_code_string + i * 3, "%02X ", *(((uint8_t *)func) + i));
                message(MESSAGE_DEBUG, "Running: %s\n", machine_code_string);
                free(machine_code_string);
        #endif

        
        asm("push rcx;    \
             mov rcx, %0;" : : "a"((uintptr_t)code->func + code->code_size) :);

        // Push variables onto the stack
        for (int i = 0; i < code->var_count; i++) {
                uint64_t bytes_double;
                double arg = arg = va_arg(args, double);
                memcpy(&bytes_double, &arg, sizeof(uint64_t));
                asm("push %0" : : "a"(bytes_double) :);
        }

        double result = ((double (*) (void))func)();        

        // Pop variables off the stack
        for (int i = 0; i < code->var_count; i++)
                asm("pop rax" : : : "rax");

        asm("pop rcx");

        // pthread_mutex_unlock(&run_mutex);

        return result;
}

void set_lexer(int (*func)(token_t *))
{
        _set_lexer_function(func);
}

void set_parser(tree_code_t * (*func)())
{
        _set_parser_function(func);
}

void set_semantic_analyzer(void (*func)(tree_code_t *))
{
        _set_validate_function(func);
}

void set_code_generator(code_block_t (*func)(tree_code_t *, int))
{
        code_generator = func;
}

void set_message_handler(void (*func)(int, const char *, va_list))
{
       _set_message_handler(func); 
}

void set_run_function(double (*func)(code_block_t *, va_list))
{
        run_func = func;
}