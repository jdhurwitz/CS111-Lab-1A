// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include <stdio.h>
#include <ctype.h>
#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

//Structs (nodes) for tokens and commands
struct cmd_node{
  struct command *cmd;
  struct cmd_node *next_node;
};

struct cmd_node_list{
  struct cmd_node *head_node;
  struct cmd_node *cur_node;
};

struct token_node{
  char *token;
  struct token_node *next_node;
};

struct token_node_list{
  struct token_node *head_node;
  struct token_node *cur_node;
};


/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */


int isWord(char stream_input){
  if(isalnum(stream_input))
    return 1;
  switch(stream_input){
    case '!':
    case '%':
    case '+':
    case ',':
    case '-':
    case '.':
    case '/':
    case ':':
    case '@':
    case '^':
    case '_':
      return 1;
    default: 
      return 0;    
   }
}


struct token_node *next_token(struct token_node_list *list){
  //Make sure it isn't the tail or head node in single node case
  if(list == NULL)
    error(1,0, "Error in next_token, token_node_list ptr NULL.");

  if(list->cur_node != NULL)
    return list->cur_node->next_node;
  else
    return NULL;
} 


struct token_node *add_token(struct token_node_list *tokens, char *token_to_add){
  int token_node_size = sizeof(struct token_node);
  //Check to see if the list is empty, and add if it is
  if(tokens->head_node == NULL){ 
    struct token_node * new_token = malloc(token_node_size);
    tokens->head_node = new_token;
    //Since there is no next node, take care of next pointer
    tokens->head_node->next_node = NULL;
    tokens->head_node->token = new_token;
    //The head node and current node are the same for the single node case
    tokens->cur_node = tokens->head_node;
    
    //return
    return tokens->cur_node;
  }else{ //There are other nodes
    struct token_node * tailptr = malloc(token_node_size);
    tailptr->next_node = NULL;     //the tail points to nothing
    tailptr->token = token_to_add; //Add pointer to the character we want to add
    tokens->cur_node->next_node = tailptr;
    tokens->cur_node = tokens->cur_node->next_node; //Move to new node

    return tokens->cur_node;

  }

}

struct cmd_node * add_cmd_node (struct cmd_node_list * c_list, struct command * n_command) {
  int node_size = sizeof(struct cmd_node);

  if (c_list->head == NULL){
    struct cmd_node * n_node = malloc (node_size);
    c_list->head = n_node;
    c_list->head->next = NULL;
    c_list->head->command = n_command;
    c_list->current = c_list->head;
    return c_list->current;

  } else {
    struct cmd_node * n_prev = malloc(node_size);
    n_prev->next = NULL;
    n_prev->command = n_command;
    c_list->current->next = n_prev;
    c_list->current = c_list->current->next;
    return c_list->current;

  }

}

void remove_stack (struct stack *stack) {
  for (int i = 0; i < stack->num_contents; i++) {
    free (stack->contents[i]);
  }
  free (stack->contents);
  free (stack);
  return;
}

void push (struct stack *stack, void *val, int increase_size) {
  if (stack->num_contents == stack->max_contents) {
    stack->max_contents = stack->max_contents + increase_size;
    stack->contents = realloc (stack->contents, stack->max_contents * sizeof(void));
  }
  stack->contents[stack->num_contents] = val;
  stack->num_contents = stack->num_contents + 1;
  return;
}


void* view_top (struct stack *stack) {
  if (stack->num_contents == 0) {
    return NULL;
  } else {
    void *top_val = stack->contents[stack->num_contents-1];
    return top_val;
  }
}

enum precedence_list{
  IF_P,
  THEN_P,
  DO_P,
  ELSE_P,
  OPEN_PAR_P,
  SEMICOL_P,
  PIPE_P,
  OUTPUT_P,
  INPUT_P,
  FI_P,
  DONE_P,
  CLOSE_PAR_P,
  ERROR_P
};

/*
Create the appearance of a stack in order to hold the commands.
 */
struct stack{
  int num_contents;
  int max_contents;
  void** contents;
}

struct stack* create_stack(int specified_max){
  int stack_size = sizeof(struct stack);
  struct stack* new_stack = malloc(stack_size);
  new_stack->max_contents = specified_max; //Set max amt in stack
  new_stack->num_contents = 0; //Nothing in the stack atm

  int contents_alloc_size = new_stack->max_contents * sizeof(void*);
  new_stack->contents = malloc(contents_alloc_size); 
  return new_stack;
}

struct stack* increase_stack_max(struct stack* user_stack, int new_max){
  user_stack->max_contents = new_max;
}


void* pop(struct stack *user_stack){
  if(user_stack->max_contents <=0 || user_stack->num_contents <= 0){
    return NULL;
  }
  else{
    int new_num_contents = num_contents -1;
    user_stack->num_contents = new_num_contents;
    void* modified_stack = user_stack->contents[user_stack->num_contents];
    return modified_stack;
  }
  return NULL;

}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
