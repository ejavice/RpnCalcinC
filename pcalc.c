#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcalc.h"


/*Register Methods*/
registers CurrentRegisters;
registers *get_registers_ptr(){
	return (&CurrentRegisters);
}
/*End of Register Methods*/






/*STACK METHODS*/
stack_elt *push_reg (stack_elt *first_elt, int regis, instructions *first_instruction){
	registers *CurrentRegisters = get_registers_ptr();
	// Allocate a new list element with malloc
	stack_elt *elt;
	elt = malloc (sizeof(*elt));
	// If malloc fails end the program
	if (elt == NULL) {
		printf ("Exception: Couldn't allocate a new stack element\n");
		destroy_stack(first_elt);
		destroy_instructions(first_instruction);
		exit (1);
	}
	elt->data = CurrentRegisters->R[regis];
	elt->next = first_elt;
	return elt;
}

stack_elt *pop_reg (stack_elt *first_elt, int regis, instructions *first_instruction){
	registers *CurrentRegisters = get_registers_ptr();
	//if there is nothing on the stack
	if(first_elt==NULL){
		printf ("Exception: Couldn't not pop from an empty stack\n");
		destroy_stack(first_elt);
		destroy_instructions(first_instruction);
		exit (1);
	}
	CurrentRegisters->R[regis]=first_elt->data;
	stack_elt *next = first_elt->next;
	free(first_elt);
	//return pointer to the next of the stack
	return next;
}

stack_elt *push_int (stack_elt *first_elt, int num, instructions *first_instruction){
	// Allocate a new list element with malloc
	stack_elt *elt;
	elt = malloc (sizeof(*elt));
	// If malloc fails end the program
	if (elt == NULL) {
		printf ("Exception: Couldn't allocate a new stack element\n");
		destroy_stack(first_elt);
		destroy_instructions(first_instruction);
		exit (1);
	}
	elt->data = num;
	elt->next = first_elt;
	return elt;
}

stack_elt *pop_int (stack_elt *first_elt, instructions *first_instruction){
	//if there is nothing on the stack
	if(first_elt==NULL){
		printf ("Exception: Trying to pop from an empty stack\n");
		destroy_stack(first_elt);
		destroy_instructions(first_instruction);
		exit (1);
	}
	stack_elt *next = first_elt->next;
	free(first_elt);
	//return pointer to the next of the stack
	return next;
}

int peek(stack_elt *first_elt, instructions *first_instruction){
	if(first_elt==NULL){
		printf("Exception: Access on an empty stack\n");
		destroy_stack(first_elt);
		destroy_instructions(first_instruction);
		exit(1);
	}else{
		return first_elt->data;
	}
}

void destroy_stack(stack_elt *first_elt){
	while(first_elt!=NULL){
		stack_elt *next = first_elt->next;
		free(first_elt);
		first_elt=next;
	}
}
/*End of STACK METHODS*/






/*Instructions METHODS*/
instructions *add_commands(instructions *previous_command, char *file_name){
	FILE *file;
	char buffer[BUFSIZ];
	char *command;
	int i =0;
	instructions *first_instruction;
	//open file
	file = fopen(file_name, "r");
	while (fgets(buffer, sizeof(buffer), file)) {
		instructions *elt;
		// Allocate a new instruction node
		elt = malloc (sizeof(*elt));
		if (elt == NULL) {
			printf ("Couldn't allocate a new instruction node\n");
			destroy_instructions(first_instruction);
			exit (1);
		}
		//Allocate space for instruction
		command = strtok(buffer, "\n");
		elt->instruction = (char *)malloc(strlen(command) + 1);
		if (elt->instruction == NULL) {
			printf ("Couldn't allocate a new instruction string\n");
			destroy_instructions(first_instruction);
			exit (1);
		}
		strcpy (elt->instruction, command);
		
		if(i==0){
			elt->prev = NULL;
			first_instruction = elt;
		}else{
			previous_command->next = elt;
			elt->prev = previous_command;
			elt->next = NULL;
		}
		previous_command = elt;
		i++;
	}
	return first_instruction;
}

void check_no_duplicate_loop_names(instructions *first_instruction){
	instructions *current = first_instruction;
	instructions *prev = NULL;
	instructions *next = first_instruction->next;
	while(current){
		size_t length = strlen(current->instruction);
		char temp[length+1];
		char *subbuffer;
		strcpy(temp, current->instruction);
		subbuffer = strtok(temp, " \n\t");
		if(strcmp("LABEL", subbuffer)==0){
			prev = current->prev;
			while(prev){
				if(strcmp(prev->instruction,current->instruction)==0){
					printf("Exception: Multiple Labels with same name: %s \n", current->instruction);
					destroy_instructions(first_instruction);
					exit(1);
				}
				prev = prev->prev;
			}
			next = current->next;
			while(next){
				if(strcmp(next->instruction,current->instruction)==0){
					printf("Exception: Multiple Labels with same name: %s \n", current->instruction);
					destroy_instructions(first_instruction);
					exit(1);
				}
				next = next->next;
			}
		}
		current = current->next;
	}
}

void destroy_instructions(instructions *first_instruction){
	while(first_instruction!=NULL){
		instructions *next = first_instruction->next;
		free(first_instruction->instruction);
		free(first_instruction);
		first_instruction=next;
	}
}
/*End of Instructions MEthods*/






/*READ FILE METHODS*/
void proper_file_format_check(char *file_name){
	FILE *file;
	char buffer[BUFSIZ];
	char *subbuffer;
	//open file
	file = fopen(file_name, "r");
	//check if file exists
	if (file == NULL) {
		printf("Exception: File not found. \n");
		exit(1);
	}
	//check proper file extension
	const char *dot = strrchr(file_name, '.');
    if(!dot || dot == file_name){
    	printf("Exception: No file extension or file name.\n");
		exit(1);
    } 
	if(strcmp(".rpn", dot)!=0){
		printf("Exception: Invalid file extension: %s \n", dot+1);
		exit(1);
	}
	//read line through line
	while (fgets(buffer, sizeof(buffer), file)) {
		subbuffer = strtok(buffer, " \n\t");
		if(strcmp("CONST",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			check_integer_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("PUSH",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("POP",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("PRINTNUM",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("ADD",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("SUB",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("MPY",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("DIV",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("MOD",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("LABEL",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else if(strcmp("BRANCHn",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);	
		}else if(strcmp("BRANCHz",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);		
		}else if(strcmp("BRANCHp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);				
		}else if(strcmp("BRANCHnz",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);	
		}else if(strcmp("BRANCHnp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);	
		}else if(strcmp("BRANCHzp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);	
		}else if(strcmp("BRANCHnzp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);	
		}else if(strcmp("JSR",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);	
		}else if(strcmp("JMPR",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			check_register_format(subbuffer);
			subbuffer = strtok(NULL, " \n\t");
			check_no_more_headers(subbuffer);
		}else{
			printf("Exception: Invalid input file format: subbuffer: %s \n", subbuffer);
			exit(1);
		}
	}
	fclose(file);
}

void check_register_format(char *subbuffer){
	if((strcmp("R0",subbuffer)==0)||(strcmp("R1",subbuffer)==0)||(strcmp("R2",subbuffer)==0)||(strcmp("R3",subbuffer)==0)||(strcmp("R4",subbuffer)==0)||(strcmp("R5",subbuffer)==0)||(strcmp("R6",subbuffer)==0)||(strcmp("R7",subbuffer)==0)){
		//all good
	}else{
		printf("Exception: Invalid File Format - Register : %s does not exist. \n", subbuffer);
		exit(1);
	}
}

void check_integer_format(char *subbuffer){
	long int number_check;
	char *number_check_ptr;
	char number_check_string[strlen("-2147483648")+1]; // max necessary for biggest int

	//check if const specified
	if(subbuffer==NULL){
		printf("Exception: Invalid File Format - No specified value for constant to store \n");
		exit(1);
	}
	//remove leading zeros
	while(strlen(subbuffer)>2){
		if (subbuffer[0] == '0') 
			memmove(subbuffer, subbuffer+1, strlen(subbuffer));
		else{
			break;
		}
	}

	number_check = strtod(subbuffer, &number_check_ptr);
	//check that is an int form
	sprintf(number_check_string, "%d", (int)number_check);
	if (memcmp (subbuffer, number_check_string, strlen(subbuffer)-1) != 0){
		printf("Exception: Invalid File Format - Not an integer to store in register: %s \n", subbuffer);
		exit(1);
	}
	//check if within bounds of integer
	if(number_check<-2147483647){
		printf("Exception: Invalid File Format - Value for constant to store not in bounds of integer: %ld \n",number_check);
		exit(1);
	}else if(number_check>2147483647){
		printf("Exception: Invalid File Format - Value for constant to store not in bounds of integer: %ld \n",number_check);
		exit(1);
	}
}

void check_no_more_headers(char *subbuffer){
	if(subbuffer!=NULL){
		if(subbuffer[0]!=';'){		//; is for comments
			printf("Exception: Invalid File Format - Too many headers in instructions: %s \n", subbuffer);
			exit(1);
		}
	}
}
/*End of READ File Methods*/






/****EXECUTE CALCULATION METHOD *****/
void calculate(instructions *first_instruction){
	registers *CurrentRegisters = get_registers_ptr();
	instructions *current_instruction = first_instruction;
	stack_elt *first_elt = NULL;
	int regis;
	int number;
	int temp1;
	int temp2;
	char *label_name;
	int program_counter = 0;
	while(current_instruction!=NULL) {
		//printf("instruction: %s -- pc: %d \n", current_instruction->instruction, program_counter);
		size_t length = strlen(current_instruction->instruction);
		char temp[length+1];
		char *subbuffer;
		strcpy(temp, current_instruction->instruction);
		subbuffer = strtok(temp, " \n\t");

		if(strcmp("CONST",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			number = atoi(subbuffer);
			CurrentRegisters->R[regis] = number;
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("PUSH",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			first_elt = push_reg(first_elt, regis, first_instruction);
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("POP",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			first_elt = pop_reg(first_elt, regis, first_instruction);
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("PRINTNUM",subbuffer)==0){
			printf("%d\n",peek(first_elt, first_instruction));
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("ADD",subbuffer)==0){
			temp1 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			temp2 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			first_elt = push_int(first_elt, (temp1+temp2), first_instruction); 
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("SUB",subbuffer)==0){
			temp1 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			temp2 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			first_elt = push_int(first_elt, (temp1-temp2), first_instruction); 
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("MPY",subbuffer)==0){
			temp1 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			temp2 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			first_elt = push_int(first_elt, (temp1*temp2), first_instruction); 
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("DIV",subbuffer)==0){
			temp1 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			temp2 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			first_elt = push_int(first_elt, (temp1/temp2), first_instruction); 
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("MOD",subbuffer)==0){
			temp1 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			temp2 = peek(first_elt, first_instruction);
			first_elt = pop_int(first_elt, first_instruction);
			first_elt = push_int(first_elt, (temp1%temp2), first_instruction); 
			
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("LABEL",subbuffer)==0){
			current_instruction = current_instruction->next;
			program_counter++;
		}else if(strcmp("BRANCHn",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			if(CurrentRegisters->R[regis]<0){
				program_counter = 0;
				current_instruction=first_instruction;
				while(current_instruction){
					size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
					subbuffer = strtok(temp, " \n\t");
					if(strcmp("LABEL",subbuffer)==0){
						subbuffer = strtok(NULL, " \n\t");
						if(strcmp(label_name,subbuffer)==0){
							goto breakin_it;
						}
					}
					current_instruction = current_instruction->next;
					program_counter++;
				}
				if(current_instruction==NULL){
					printf("Exception: Unable to find LOOP with label: %s \n", label_name);
					destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
				}
			}else{
				current_instruction = current_instruction->next;
				program_counter++;
			}
		}else if(strcmp("BRANCHz",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			if(CurrentRegisters->R[regis]==0){
				program_counter = 0;
				current_instruction=first_instruction;
				while(current_instruction){
					size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
					subbuffer = strtok(temp, " \n\t");
					if(strcmp("LABEL",subbuffer)==0){
						subbuffer = strtok(NULL, " \n\t");
						if(strcmp(label_name,subbuffer)==0){
							goto breakin_it;
						}
					}
					current_instruction = current_instruction->next;
					program_counter++;
				}
				if(current_instruction==NULL){
					printf("Exception: Unable to find LOOP with label: %s \n", label_name);
					destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
				}
			}else{
				current_instruction = current_instruction->next;
				program_counter++;
			}
		}else if(strcmp("BRANCHp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			if(CurrentRegisters->R[regis]>0){
				program_counter = 0;
				current_instruction=first_instruction;
				while(current_instruction){
					size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
					subbuffer = strtok(temp, " \n\t");
					if(strcmp("LABEL",subbuffer)==0){
						subbuffer = strtok(NULL, " \n\t");
						if(strcmp(label_name,subbuffer)==0){
							goto breakin_it;
						}
					}
					current_instruction = current_instruction->next;
					program_counter++;
				}
				if(current_instruction==NULL){
					printf("Exception: Unable to find LOOP with label: %s \n", label_name);
					destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
				}
			}else{
				current_instruction = current_instruction->next;
				program_counter++;
			}
		}else if(strcmp("BRANCHnz",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			if(CurrentRegisters->R[regis]<=0){
				program_counter = 0;
				current_instruction=first_instruction;
				while(current_instruction){
					size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
					subbuffer = strtok(temp, " \n\t");
					if(strcmp("LABEL",subbuffer)==0){
						subbuffer = strtok(NULL, " \n\t");
						if(strcmp(label_name,subbuffer)==0){
							goto breakin_it;
						}
					}
					current_instruction = current_instruction->next;
					program_counter++;
				}
				if(current_instruction==NULL){
					printf("Exception: Unable to find LOOP with label: %s \n", label_name);
					destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
				}
			}else{
				current_instruction = current_instruction->next;
				program_counter++;
			}
		}else if(strcmp("BRANCHnp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			if(CurrentRegisters->R[regis]!=0){
				program_counter = 0;
				current_instruction=first_instruction;
				while(current_instruction){
					size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
					subbuffer = strtok(temp, " \n\t");
					if(strcmp("LABEL",subbuffer)==0){
						subbuffer = strtok(NULL, " \n\t");
						if(strcmp(label_name,subbuffer)==0){
							goto breakin_it;
						}
					}
					current_instruction = current_instruction->next;
					program_counter++;
				}
				if(current_instruction==NULL){
					printf("Exception: Unable to find LOOP with label: %s \n", label_name);
					destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
				}
			}else{
				current_instruction = current_instruction->next;
				program_counter++;
			}
		}else if(strcmp("BRANCHzp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			if(CurrentRegisters->R[regis]>=0){
				program_counter = 0;
				current_instruction=first_instruction;
				while(current_instruction){
					size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
					subbuffer = strtok(temp, " \n\t");
					if(strcmp("LABEL",subbuffer)==0){
						subbuffer = strtok(NULL, " \n\t");
						if(strcmp(label_name,subbuffer)==0){
							goto breakin_it;
						}
					}
					current_instruction = current_instruction->next;
					program_counter++;
				}
				if(current_instruction==NULL){
					printf("Exception: Unable to find LOOP with label: %s \n", label_name);
					destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
				}
			}else{
				current_instruction = current_instruction->next;
				program_counter++;
			}
		}else if(strcmp("BRANCHnzp",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			program_counter = 0;
			current_instruction=first_instruction;
			while(current_instruction){
				size_t length = strlen(current_instruction->instruction);
					char temp[length+1];
					char *subbuffer;
					strcpy(temp, current_instruction->instruction);
				subbuffer = strtok(temp, " \n\t");
				if(strcmp("LABEL",subbuffer)==0){
					subbuffer = strtok(NULL, " \n\t");
					if(strcmp(label_name,subbuffer)==0){
						goto breakin_it;
					}
				}
				current_instruction = current_instruction->next;
				program_counter++;
			}
			if(current_instruction==NULL){
				printf("Exception: Unable to find LOOP with label: %s \n", label_name);
				destroy_stack(first_elt);
				destroy_instructions(first_instruction);
				exit(1);
			}	
		}else if(strcmp("JSR",subbuffer)==0){
			program_counter++;
			first_elt = push_int(first_elt, program_counter, first_instruction);

			subbuffer = strtok(NULL, " \n\t");
			label_name = subbuffer;
			program_counter = 0;
			current_instruction=first_instruction;
			while(current_instruction){
				size_t length = strlen(current_instruction->instruction);
				char temp[length+1];
				char *subbuffer;
				strcpy(temp, current_instruction->instruction);
				subbuffer = strtok(temp, " \n\t");
				if(strcmp("LABEL",subbuffer)==0){
					subbuffer = strtok(NULL, " \n\t");
					if(strcmp(label_name,subbuffer)==0){
						goto breakin_it;
					}
				}
				current_instruction = current_instruction->next;
				program_counter++;
			}
			if(current_instruction==NULL){
				printf("Exception: Unable to find LOOP with label: %s \n", label_name);
				destroy_stack(first_elt);
				destroy_instructions(first_instruction);
				exit(1);
			}
		}else if(strcmp("JMPR",subbuffer)==0){
			subbuffer = strtok(NULL, " \n\t");
			regis = subbuffer[1] - 48;
			if(CurrentRegisters->R[regis]<0){
				printf("Exception: Unable to Jump to negative counter : %d \n", CurrentRegisters->R[regis]);
				destroy_stack(first_elt);
					destroy_instructions(first_instruction);
					exit(1);
			}else{
				current_instruction = first_instruction;
				program_counter = CurrentRegisters->R[regis];
				for(int h=0; h <= program_counter; h++){
					current_instruction = current_instruction->next;
					if(current_instruction == NULL){
						printf("Exception: Unable to Jump to too large counter: %d \n", CurrentRegisters->R[regis]);
						destroy_stack(first_elt);
						destroy_instructions(first_instruction);
						exit(1);
					}
				}
				
			}
		}
		breakin_it:;
	}
	destroy_stack(first_elt);
}
/****END OF CALCULATION METHOD ****/






/*DEBUGGING METHODS */
void list_commands(instructions *first_instruction){
	instructions *elt = first_instruction;
	while(elt){
		printf("%s\n", elt->instruction);
		elt = elt->next;
	}
}

void list_stack(stack_elt *first_elt){
	stack_elt *elt = first_elt;
	while(elt){
		printf("%d\n", elt->data);
		elt = elt->next;
	}
}
/*End of DEBUGGING METHODS*/






/*MAIN FUNCTION*/
int main(int argc, char *argv[]){
	
	//check if correct number of arguments
	if(argc != 2){
		printf("Exception: Invalid number of arguments\n");
		return 1;
	}

	//Check if proper file format
	proper_file_format_check(argv[1]);

	//add commands
	instructions *first_instruction = NULL;
	first_instruction = add_commands(first_instruction, argv[1]);

	//check duplicate label assignments
	check_no_duplicate_loop_names(first_instruction);

	//Execute Commands
	calculate(first_instruction);
	
	//free memory
	destroy_instructions(first_instruction);
	return 0;
}
/*END of Main FUNCTION*/







