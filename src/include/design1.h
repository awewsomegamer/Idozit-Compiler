#ifndef DESIGN1_H
#define DESIGN1_H

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/*
NUMBER = 0123456789
IDENTIFIER = string of characters that do not include numbers

program: addition

addition: multiplication + multiplication
        | multiplication - multiplication

multiplication: term * term
              | term / term

term: ( expression )
    | NUMBER
    | variable
    | function

variable: IDENTIFIER != reserved

function: IDENTIFIER == reserved
        & IDENTIFIER NUMBER ( addition )

*/

// Mathematical Constants
#define PI                                       3.14159265358979323846
#define SQR_2                                    1.41421356237309504880
#define SQR_3                                    1.73205080756887729352
#define SQR_5                                    2.23606797749978969640
#define GOLDEN_RATIO                             1.61803398874989484820
#define SILVER_RATIO                             2.41421356237309504880
#define CBR_2                                    1.25992104989487316476 // Cube Root of 2
#define CBR_3                                    1.44224957030740838232 // Cube Root of 3
#define R12_2                                    1.05946309435929526456 // 12th Root of 2
#define SUPER_GOLD_RATIO                         1.46557123187676802665
#define CONNECTIVE_CONST                         1.84775906502257351225
#define KEPLER_BOUWKAMP_CONST                    0.11494204485329620070 
#define WALLIS_CONSTANT                          2.09455148154232659148
#define EULERS_NUMBER                            2.71828182845904523536 
#define LN_2                                     0.69314718055994530941
#define LEMNISCATE_CONST                         2.62205755429211981046
#define EULERS_CONSTANT                          0.57721566490153286060
#define ERDOS_BORWEIN_CONST                      1.60669515241529176378
#define OMEGA_CONST                              0.56714329040978387299
#define APERY_CONST                              1.20205690315959428539
#define LALPLACE_LIMIT                           0.66274341934918158097
#define RAMANUJAN_SOLDNER_CONST                  1.45136923488338105028
#define GAUSS_CONST                              0.83462684167407318628
#define SECOND_HERMITE_CONST                     1.15470053837925152901
#define LIOUVILLE_CONST                          0.110001000000000000000001
#define FIRST_CONTINUED_FRACT_CONST              0.69777465796400798201
#define RAMANUJAN_CONST                          262537412640768743.999999999999250073 
#define GLAISHER_KINKELIN_CONST                  1.28242712910062263687
#define CATALAN_CONST                            0.91596559417721901505
#define DOTTIE_NUMBER                            0.73908513321516064165
#define MEISSEL_MERTENS_CONST                    0.26149721284764278375
#define UNIVERSAL_PARABOLIC_CONST                2.29558714939263807403
#define CAHEN_CONST                              0.64341054628833802618
#define GELFOND_CONST                            23.1406926327792690057
#define GELFOND_SCHNEIDER_CONST                  2.66514414269022518865
#define SECOND_FAVARD_CONST                      1.23370055013616982735
#define GOLDEN_ANGLE                             2.39996322972865332223
#define SIERPINSKI_CONST                         2.58498175957925321706
#define LANDAU_RAMANUJAN_CONST                   0.76422365358922066299
#define FIRST_NIELSEN_RAMANUJAN_CONST            0.82246703342411321823
#define GIESEKING_CONST                          1.01494160640965362502
#define BERNSTEIN_CONST                          0.28016949902386913303
#define TRIBONACCI_CONST                         1.83928675521416113255
#define BRUN_CONST                               1.902160583104
#define TWIN_PRIMES_CONST                        0.66016181584686957392
#define PLASTIC_NUMBER                           1.32471795724474602596
#define BLOCH_CONST(a)                           (0.4332 <= a <= 0.4719)
#define Z_SCORE_975_PRECENTILE                   1.95996398454005423552
#define LANDAU_CONST(a)                          (0.5 <= a <= 0.545326)
#define LANDAU_THIRD_CONST(a)                    (0.5 <= a <= 0.7853)
#define PROUHET_THUE_MORSE_CONST                 0.41245403364010759778
#define GOLOMB_DICKMAN_CONST                     0.62432998854355087099
#define LABESGUE_CONSTS                          0.98943127383114695174
#define FELLER_TORNIER_CONST                     0.66131704946962233528
#define CHAMPERNOWNE_CONST                       0.12345678910111213141
#define SALEM_CONST                              1.17628081825991750654
#define KHINCHINS_CONST                          2.68545200106530644530
#define LEVY_CONST_1                             1.18656911041562545282
#define LEVY_CONST_2                             3.27582291872181115978
#define COPELAND_ERDOS_CONST                     0.23571113171923293137
#define MILLS_CONST                              1.30637788386308069046
#define GOMPERTZ_CONST                           0.59634736232319407434 
#define DE_BRUJIN_NEWMAN_CONST(a)                (0 <= a <= 0.2)
#define VAN_DER_PAUW_CONST                       4.53236014182719380962
#define MAGIC_ANGLE                              0.955316618124509278163
#define ARTIN_CONST                              0.37395581361920228805
#define PORTER_CONST                             1.46707807943397547289
#define LOCHS_CONST                              0.97027011439203392574
#define DEVICCI_TESSERACT_CONST                  1.00743475688427937609
#define LIEB_SQUARE_ICE_CONST                    1.53960071783900203869
#define NIVENS_CONST                             1.70521114010536776428
#define STEPHENS_CONST                           0.57595996889294543964
#define REGULAR_PAPERFOLDING_SEQ                 0.85073618820186726036
#define RECIPROCAL_FIBONACCI_CONST               3.35988566624317755317
#define CHVATAL_SANKOFF_CONST(a)                 (0.788071 <= a <= 0.826280)
#define FEIGENBAUM_CONST                         4.66920160910299067185
#define CHAITIN_CONSTS                           0.0078749969978123844  // Only one number of the constants
#define ROBBINS_CONST                            0.66170718226717623515
#define WEIERSTRASS_CONST                        0.47494937998792065033
#define FRANSEN_ROBINSON_CONST                   2.80777024202851936522 
#define FEIGENBAUM_CONST_ALPHA                   2.50290787509589282228
#define SECOND_DU_BOIS_REYMOND_CONST             0.19452804946532511361
#define ERDOS_TENENBAUM_FORD_CONST               0.86071332055934206887
#define CONWAY_CONST                             1.30357726903429639125
#define HAFNER_SARNAK_MCCURELEY_CONST            0.35323637185499598454
#define BACKHOUSE_CONST                          1.45607494858268967139
#define VISWANATH_CONST                          1.1319882487943
#define KOMORNIK_LORETI_CONST                    1.78723165018296593301
#define EMBREE_TREFETHEN_CONST                   0.70258
#define HEATH_BROWN_MOROZ_CONST                  0.00131764115485317810
#define MRB_CONST                                0.18785964246206712024
#define PRIME_CONST                              0.41468250985111166024
#define SOMOS_QUADRATIC_RECURRENCE_CONST         1.66168794963359412129
#define FOAIS_CONST                              1.18745235112650105459
#define LOGARITHMIC_CAPACITY_UNIT_DISK           0.59017029950804811302
#define TANIGUCHI_CONST                          0.67823449191739197803
                                                 
// Compiler Flags (Set: 1, Clear: 0)
#define NAME_FLAG_ERRPASS 1 // When clear, compiler will break when an error is encountered (Default: set)

// Helpful
#define FUNCTION(name, func, args) uint64_t (*name)args = func; // FUNCTION(my_func, func, (int, int, int))
#define CALL(name) (*name)                                      // CALL(my_func)(5,112,4)
#define DEBUG 0

/* struct tree_code :
 * An intermediate structure.
 *
 * value        : An integer value
 * type         : The type of the current node (+, -, /, *, etc...)
 * used         : How many branches are used
 * left & right : Branches to the left and right
 */
struct tree_code
{
    double value;
    uint8_t type;

    struct tree_code *left;
    struct tree_code *right;
};
typedef struct tree_code tree_code_t;

/* struct context
 * A context for the compiler to compile
 * Contains the head of the tree structure and its symbols
 */
struct context
{
    tree_code_t *head;
    char **symbols;
};
typedef struct context context_t;

/*
 * struct code_block :
 * A structure representing a compiled block of machine code.
 *
 * func  : A pointer to the function to call to run the equation
 * data  : A pointer to the data section of the function
 * size  : Number of generated bytes (size of program)
 */
struct code_block
{
    uint8_t *func;
    uint8_t *data;
    size_t code_size;
    size_t data_size;
};
typedef struct code_block code_block_t;

/* struct compiler_params :
 * optimization_level : The level of optimization the compiler should do (0: none, 3: optimize everything)
 * break_on_err       : Should the compiler break when it encounters an error (0: no, 1: yes)
 * debug              : A pointer to the function the compiler should call for debug messages 
 *                      (void debug(const char *form, ...);) (NULL == no print)
 * error              : A pointer to the function the compiler should call for error messages 
 *                      (void error(const char *form, ...);) (NULL == no print)
 */
struct compiler_params
{
    uint8_t optimization_level;
    uint8_t flags;
    void *debug;
    void *error;
};
typedef struct compiler_params compiler_params_t;

/* expression(const char *form, ...) :
 * Converts the given form into tree code (intermediate representation).
 * Variables within the equation are provided as strings in the ...
 * The function will return a context structure which is
 * used in compilation
 */
context_t expression(const char *form, char **variables);

/* code_block_t compile(tree_code_t *tree) :
 * Compile the given context using the set generator
 * and return it in a code_block_t structure.
 */
code_block_t compile(context_t ctx);

double run(code_block_t code, double *variables);

/* tree_to(tree_code_t *tree, const char *language) :
 * Takes the generated tree, tree, and converts it into a given language,
 * language.
 */
const char *tree_to(tree_code_t *tree, const char *language);

/* objectify(FILE *file, code_block_t code, ...) :
 * Convert generated code (code, ...) into object format and write to
 * the given file, file.
 */
void objectify(FILE *file, code_block_t code, ...);

/* void set_message_handler(void*) :
 * This function will set the message handler
 * to the given void* (function).
 */
void set_message_handler(void (*func)(int, const char *, va_list));

/* void default_message_handler() :
 * This function will revert the message handler
 * to the default message handler. 
 */
void default_message_handler();

/* void set_code_generator(code_bloc_t (*generator)(tree_code_t*)) :
 * Sets the code generator to the given code generator
 */
void set_code_generator(code_block_t (*generator)(tree_code_t *));

/* void default_code_generator() :
 * Sets the code generator to the default generator.c (which is for x86-64)
 */
void default_code_generator();

#endif