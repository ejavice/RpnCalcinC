/*
 * pcalc.h : Declares functions used to implement a the calculator
 */


/*MAKE THE STRUCKS for REGISTERS, STACK, and INSTRUCTIONS*/

//struct for the instructions
typedef struct instructions_tag {
  char *instruction;
  struct instructions_tag *prev, *next; // Pointers to the previous and next elements in the list
} instructions;


//struct for the registers
typedef struct {
	int R[8];
} registers;

//struct for the stack
typedef struct stack_elt_tag {
	int data;
	struct stack_elt_tag *next, *prev; //pointer to next element of the stack
} stack_elt;

/*End of STRUCKS*/







/*STACK METHODS*/ //WE PASS IN first_instruction to exit cleanly in case of error

//pop the next element of the stack with registers
stack_elt *pop_reg (stack_elt *first_elt, int regis, instructions *first_instruction);
//push an element to the next of the stack with register
stack_elt *push_reg (stack_elt *first_elt, int regis, instructions *first_instruction);


//pop the next element of the stack with immediate
stack_elt *pop_int (stack_elt *first_elt, instructions *first_instruction);
//push an element to the next of the stack with immediate
stack_elt *push_int (stack_elt *first_elt, int num, instructions *first_instruction);


//looks at num at top of the stack
int peek(stack_elt *first_elt, instructions *first_instruction);

//destroys the entire stack
void destroy_stack(stack_elt *first_elt);

/*End of Stack METHODS*/








/*Register Methods*/

//initialize the registers
registers *get_registers_ptr();

/*End of Registers METHODS*/







/*Instruction Methods */

//add a comand to instruction list
instructions *add_commands(instructions *previous_command, char *file_name);

//check for no duplicate label names
void check_no_duplicate_loop_names(instructions *first_instruction);

//destroy the entire instruction list
void destroy_instructions(instructions *first_instruction);

/*End of Instruction Methods */






/*Calculator Execution Method*/

void calculate(instructions *first_instruction);

/*End of Calculator Execution Method*/







/* Helper METHODS*/

void proper_file_format_check(char *file_name);
void check_register_format(char *subbuffer);
void check_integer_format(char *subbuffer);
void check_no_more_headers(char *subbuffer);

/*End of Helper METHODS*/







/*Debugging Methods*/
void list_commands(instructions *first_instruction);
void list_stack(stack_elt *first_elt);



