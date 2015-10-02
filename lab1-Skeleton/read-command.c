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

int is_operator(char test){
  //-1 = unknown, 0 = not word, 1 = word
  if(test == ' '){ //White space
    return 0;
  }else if( test == '\n')
    return 1;

 switch(test){
  case '(':
  case ')':
  case '<':
  case '>':
  case ';':
  case '|':
   return 1;
  default:
   return -1;
  }

}

//FINISH
int check_cmd(char* cmd){
  if( is_operator(cmd[0]) == 0) //space
    return 1;
  else if(is_operator(cmd[0] == 1)
    return 0;
  
	  /*
	    case d, u, etc.
	   */
  
  return 1;
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
  THEN        = 0,
  DO          = 1,
  ELSE        = 2,
  OPEN_PAR    = 3,
  SEMICOL     = 4,
  PIPE        = 5,
  OUTPUT_REDIR= 6,
  INPUT_REDIR = 7,
  DONE        = 8,
  CLOSE_PAR   = 9,
  ERROR       = 10
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

command_t combine (command_t first, command_t second, char *operand) {
  command_t combined = malloc (sizeof (struct command));
  combined->input=NULL;
  combined->u.command[1]=second;
  combined->u.command[0]=first;
  combined->status=-1;
  combined->output=NULL;
  int length;
  if (operand[0] == '>' || operand[0] == '<'){
    switch (operand[0]) {
    case '>':
      combined->input = first->input;
      length = strlen(second->u.word[0]);
      combined->output=malloc(sizeof(char)*(len+1));
      strcpy(combined->output, second->u.word[0]);
      break;
    case '<':
      combined->output = first->output;
      length = strlen(second->u.word[0]);
      combined->input=malloc(sizeof(char) * (len+1));
      strcpy(combined->input, second->u.word[0]);
      break;
    }
    return combined;
  }
    
  if (operand[0]=='&' && operand[1]=='&'){
      combined->type = AND_COMMAND;
  }
  if (operand[0]=='|' && operand[1]=='|'){
      combined->type = OR_COMMAND;
  }
  switch (operand[0]) {
  case ';':
    combined->type = SEQUENCE_COMMAND;
    break;
  case '\n':
    combined->type = SEQUENCE_COMMAND;
    break;
  case '|':
    combined->type = PIPE_COMMAND;
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

          
int check_for_semicolon(char* c){
    if(c[0] == ';')
        return 1;
    else if(c[0] == '\n')
        return 1;
    else
        return 0;
}
          
int is_cmd_word (char *s) {
  if (start_cmd_word(s) || mid_cmd_word(s)) {
    return 1;
  } else {
    return 0;
  }
}

command_t create_subshell_cmd(command_t c){
    int alloc_size = sizeof(struct command);
    command_t cmd = malloc(alloc_size);
    //Set type, status, and I/O for a subshell cmd
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->status = 1; //-1 if not known, or hasn't exited yet
    cmd->type = SUBSHELL_COMMAND;
    cmd->u.command[0] = c;
    cmd->u.command[1] = NULL;
    return cmd;
}
          
command_t c_simple(char *s) {
    command_t simple = malloc(sizeof(struct command));
    simple->status = -1;
    simple->input = NULL;
    simple->type = SIMPLE_COMMAND;
    simple->output = NULL;
    
    char *buf = malloc (sizeof (char) * (strlen(s) + 1));
    simple->u.word = malloc(sizeof(char*));
    
    int index, arguments;
    
    for (int i = 0; i < strlen(s); i++){
        switch (s[i]){
            case ' ':
            case '\0':
                if (index == 0){
                    index = -1;
                    break;
                } else {
                    buf[index] = '\0';
                    simple->u.word = realloc(simple->u.word, sizeof (char*) * (arguments + 1));
                    simple->u.word[arguments] = malloc (sizeof (char) * (index +1));
                    strcpy (simple->u.word[arguments], buf);
                    arguments = arguments +1;
                    index = -1;
                }
                break;
            default:
                buf[index] = s[i];
                break;
        }
        index = index +1;
    }
    simple->u.word[arguments] = NULL;
    return simple;
}
          
command_t create (struct token_node_list *list){
    list->cur_node = list->head_node;
    command_t cmd = malloc(sizeof(struct command));
    struct token_node * current_node = next_token(list);
    
    struct stack * command = create_stack(50);
    struct stack * operator = create_stack(50);
    char *t_word = current_node->token;
    while (current_node != NULL) {
        if (isWord(t_word[0])) {
            push(command, t_word, sizeof(t_word));
        }
        if (t_word[0] == '(') {
            push(command, t_word[0], sizeof(t_word[0]));
        }
        if (is_operator(t_word[0]) {
            if (operator->num_contents==0){
                push(operator, t_word[0], sizeof(t_word[0]));
            } else if (operator->num_contents!=0) {
                pop
            }
        }
    }
    
}

command_t encounter_operator(struct stack *operator_stack, struct stack *cmd_stack){
    //operator stack and command stack as parameters
              char *op = pop(operator_stack);
              void *cmd1, *cmd2;
              command_t combined_cmd = NULL;
              //Look for two commands and consolidate for precedence issues later.
              //Second command will be on the top of the stack
              switch(op[0]){
                  case '<':
                  case '>':
                  case '\n':
                  case ';':
                  case '|':
                      cmd2 = pop(cmd_stack);
                      cmd1 = pop(cmd_stack);
                      combined_cmd = combine(cmd1, cmd2, op);
                      break;
                  case '(':
                  case ')':
                      cmd1 = pop(cmd_stack);
                      combined_cmd = create_subshell_cmd(cmd1);
                      break;
                  default:
                      break;
                      
              }
              return combined_cmd; //If this is NULL, cases weren't satisfied
              
}
          
void print_token_list(struct token_node_list *t){
              struct token_node* prev_cur = t->cur_node;
              t->cur_node = t->head;
              struct token_node* token_iterator;
              while( (token_iterator = next_token(t))){
                  fprintf(stderr, "<%s>\n", token_iterator->token);
              }
              t->cur_node = prev_cur;
              return;
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
