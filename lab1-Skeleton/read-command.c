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
  IF          = 0,
  UNTIL_WHILE = 1,
  THEN        = 2,
  DO          = 3,
  ELSE        = 4,
  OPEN_PAR    = 5,
  SEMICOL     = 6,
  PIPE        = 7, 
  OUTPUT_REDIR= 8,
  INPUT_REDIR = 9,
  FI          = 10,
  DONE        = 11, 
  CLOSE_PAR   = 12,
  ERROR       = 13
};

//NOT DONE FIX THIS
int get_precedence(char *cmd_arg){
  int isCMD = -1;
  if(strlen(cmd_arg) == 1)
    isCMD = 0;
  else if(strlen(cmd_arg) < 6)
    isCMD = 1;
 
  if(strlen(cmd_arg) == 1){ //This means that we are looking at signle char
    if(*cmd_arg == '\n')
      return SEMICOL; //same precedence
    switch(*cmd_arg){
      case '(':
        return OPEN_PAR;
      case ')':
        return CLOSE_PAR;
      case ';':
        return SEMICOL;
      case '|':
        return PIPE:
      case '>':
        return OUTPUT_REDIR;
      case '<':
        return INPUT_REDIR;
      default:
	break;
    }
  }
  

}

command_t combine (command_t 1st, command_t 2nd, char *operand) {
  command_t combined = malloc (sizeof (struct command));
  combined->u.command[2]=NULL;
  combined->u.command[1]=second_comand;
  combined->u.command[0]=first_command;
  combined->status=-1;
  combined->input=NULL;
  combined->output=NULL;
  int length;
  if (operand[0] == '>' || operand[0] == '<'){
    switch (operand[0]) {
    case '>':
      combined->input = 1st->input;
      length = strlen(2nd->u.word[0]);
      combined->output=malloc(sizeof(char)*(len+1));
      strcpy(combined->output, 2nd->u.word[0]);
      break;
    case '<':
      combined->output = 1st->output;
      length = strlen(2nd->u.word[0]);
      combined->input=malloc(sizeof(char) * (len+1));
      strcpy(combined->input, 2nd->u.word[0]);
      break;
    }
    return combined;
  }
  switch (operand[0]) {
  case 'i':
    combined->type = IF_COMMAND;
    break;
  case 'u':
    combined->type = UNTIL_COMMAND;
    break;
  case ';':
    combined->type = SEQUENCE_COMMAND;
    break;
  case '\n':
    combined->type = SEQUENCE_COMMAND;
    break;
  case '|':
    combined->type = PIPE_COMMAND;
    break;
  case 'w':
    combined->type = WHILE_COMMAND;
    break;
  default:
    error (1, 0, "Command type not recognized");
    break;
  }
  return combined;
}


//Return 0 if it is made up of white space
int is_empty(char *s) {
  while (*s!= '\0') {
    if (!isspace(*s))
      return 0;
    s++;
  }
  return 1;
}

int mid_cmd_word (char *s) {
  if (strcmp(buf, "then") == 0 || strcmp(buf, "do") == 0 ||
      strcmp(buf,"fi") == 0 || strcmp(buf, "else") == 0 || strmp(buf,
								 "done") == 0) {
    return 1;
  } else {
    return 0;
  }
}

int start_cmd_word (char *s) {
  if (strcmp(buf, "until") == 0 || strcmp(buf, "if") == 0 ||
      strcmp(buf, "while") == 0) {
    return 1;
  } else {
    return 0;
  }
}

int is_cmd_word (char *s) {
  if (start_cmd_word(s) || mid_cmd_word(s)) {
    return 1;
  } else {
    return 0;
  }
}
 
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
