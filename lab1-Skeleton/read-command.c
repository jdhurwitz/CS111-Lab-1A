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
  struct command *cmd;  //This is how command is accessed
  struct cmd_node *next_node;
};

struct command_stream{
  struct cmd_node *head;
  struct cmd_node *cur_node;
  struct cmd_node *tail;
};


enum token_name{
    PIPE,
    AND,
    OR,
    SEMICOLON,
    LEFT_REDIRECT,
    RIGHT_REDIRECT,
    SUBSHELL,
    WORD
    
};
//Tokens

struct token_node{
  enum token_name token_type;
  char *token;
  struct token_node *next_node;
};

struct token_node_list{
  struct token_node *head;
  struct token_node_list *next;
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




struct token_node *next_token(struct token_node *token){
  //Make sure it isn't the tail or head node in single node case
  if(token == NULL)
    error(1,0, "Error in next_token, token ptr NULL.");

  if(token->next_node != NULL)
    return token->next_node;
  else
    return NULL;
} 

struct token_node *add_token(struct token_node_list *tokens, char *token_to_add, enum token_name type){
  int token_node_size = sizeof(struct token_node);
  struct token_node * new_token = malloc(token_node_size);
  new_token->token_name = type;
  //Check to see if the list is empty, and add if it is
  if(tokens->head == NULL){
    tokens->head = new_token;
    //Since there is no next node, take care of next pointer
    tokens->head->next_node = NULL;
    tokens->head->token = new_token;
    //The head node and current node are the same for the single node case
    tokens->cur_node = tokens->head;
    
    
    //return
    return tokens->cur_node;
  }else{ //There are other nodes
    //struct token_node * tailptr = malloc(token_node_size);
    new_token->next_node = NULL;     //the tail points to nothing
    new_token->token = token_to_add; //Add pointer to the character we want to add
    tokens->cur_node->next_node = new_token;
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
    c_list->cur_node = c_list->head;
    c_list->tail = c_list->cur_node;
    return c_list->cur_node;

  } else {
    struct cmd_node * new_node = malloc(node_size);
    new_node->next = NULL;
    new_node->command = n_command;
    c_list->cur_node->next = n_prev;
    c_list->cur_node = c_list->cur_node->next;
    c_list->tail = c_list->cur_node;
    return c_list->current;

  }

}

/*

command_t combine (command_t first, command_t second, char *operator) {
  command_t combined = malloc (sizeof (struct command));
  combined->input=NULL;
  combined->u.command[1]=second;
  combined->u.command[0]=first;
  combined->status=-1;
  combined->output=NULL;
  int length;
  if (operator[0] == '>' || operator[0] == '<'){
    switch (operator[0]) {
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
    
  if (operator[0]=='&' && operator[1]=='&'){
      combined->type = AND_COMMAND;
  }
  if (operator[0]=='|' && operator[1]=='|'){
      combined->type = OR_COMMAND;
  }
  switch (operator[0]) {
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
*/

//Function to combine commands and an operator. RETURN 1 if success, 0 if fail
int combine(struct stack* operator_stack, struct stack* command_stack){
    if(operator_stack->num_contents  == 0) //No operators
        return 0;
    else if(command_stack->num_contents == 0 || command_stack->num_contents == 1) //not enough commands
        return 0;
    command_t combined = pop(operator_stack);
    combined->input = NULL;
    combined->output = NULL;
    
    command_t second = pop(command_stack);
    command_t first  = pop(command_stack);
    
    combined->u.command[1] = second;
    combined->u.command[0] = first;
    push(operator_stack, combined, 10); //10 for realloc size
    return 1;
    
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
/*
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
*/

///////////////////////////////////////////////////////////////////
//Populate a command tree to be used for printing out commands.  //
///////////////////////////////////////////////////////////////////

/*
command_t create_tree (struct token_node_list *list){
    list->cur_node = list->head;
    command_t cmd = malloc(sizeof(struct command));
    struct token_node * current_node = next_token(list);
    
    //Create operator and command stack of size 50
    struct stack * command_stack = create_stack(50);
    struct stack * operator_stack = create_stack(50);
    
    char *t_word = current_node->token;
    while (current_node != NULL) {
        if (isWord(t_word[0])) {
            push(command_stack, t_word, sizeof(t_word));
        }
        if (t_word[0] == '(') {
            push(command_stack, t_word[0], sizeof(t_word[0]));
        }
        if (is_operator(t_word[0]) {
            if (operator_stack->num_contents==0){
                push(operator_stack, t_word[0], sizeof(t_word[0]));
            } else if (operator_stack->num_contents!=0) {
                if(t_word[0] == '|') {
                    //Stack not empty
                    while(operator_stack->num_contents != 0 && (view_top(command_stack)->command->type != SUBSHELL_COMMAND){
                        if(view_top(operator_stack)->command->type == PIPE_COMMAND){
                            //Combine commands into pipe, top of stack will be 2nd
                            command_t first_cmd = view_top(command_stack)->command;
                            pop(command_stack);
                            command_t second_cmd = view_top(command_stack)->command;
                            pop(command_stack);
                            
                            char *operator_to_combine = view_top(operator_stack)
                            combine(first_cmd, second_cmd, operator_to_combine);
                            pop(operator_stack);
                        }else {
                            break;
                        }
                    }
                          cmd_node_list->cur_node->cmd->command->type   = PIPE_COMMAND;
                          cmd_node_list->cur_node->cmd->command->input  = NULL;
                          cmd_node_list->cur_node->cmd->command->output = NULL;
                } else if (t_word[0] == '&' && t_word[1] == '&'){
                    cmd_list->cur_node->cmd->command->type= AND_COMMAND;
                    cmd_list->cur_node->cmd->command->input = NULL;
                    cmd_list->cur_node->cmd->command->output = NULL;
                    while (operator_stack->num_contents > 0 && (view_top(operator_stack)->command->type != SUBSHELL_COMMAND) {
                        if (view_top(operator_stack)->command->type == OR_COMMAND ||
                            view_top(operator_stack)->command->type == PIPE_COMMAND ||
                            view_top(operator_stack)->command->type == AND_COMMAND)
                        {
                            command_t first_cmd = view_top(command_stack)->command;
                            pop(command_stack);
                            command_t second_cmd = view_top(command_stack)->command;
                            pop(command_stack);

                            char *operator_to_combine = view_top(operator_stack);
                            combine(first_cmd, second_cmd, operator_to_combine);
                            pop(operator_stack);
                        } else {
                            break;
                        }
                }
            }
        }
    }
    
}
*/

command_t create_tree (struct token_node_list *list){
    //list->cur_node = list->head;
    if(token_node_list == NULL){
        fprintf(stderr, "\ntoken_node_list is NULL in create_tree\n");
    }
    command_t cmd = malloc(sizeof(struct command));
    command_t cmd_prev = NULL;
    struct token_node * current_node = list->head;
    
    //Create operator and command stack of size 50
    struct stack* command_stack = create_stack(50);
    struct stack* operator_stack = create_stack(50);
    
    char *t_word = current_node->token;
    while(current_node->next_node != NULL){
        
        ///////////////// Highest priority, so can avoid checking I/O, etc.
        //////PIPE///////
        /////////////////
        if(current_node->token_type == PIPE){
            cmd->type = PIPE_COMMAND;
            
            
        }
        
        ///////////////// Lowest precedence, along with newline
        ////SEMICOLON////
        /////////////////
        if(current_node->token_type == SEMICOLON){
            cmd->type = SEQUENCE_COMMAND;
            if(combine(operator_stack, command_stack) == 1)
                push(operator_stack,cmd ,10);
            else{
                fprintf(stderr, "\n Either operand stack has fewer than 2 ops or operator stack is empty.\n");
            }
                
        }
        /////////////////
        //LEFT REDIRECT//
        /////////////////
        if(current_node->token_type == LEFT_REDIRECT){
            if(cmd_prev->type != SIMPLE_COMMAND || cmd_prev->type != SUBSHELL_COMMAND){ //simple command = word
                fprintf(stderr, "\nFormatting issue with left redirect.\n");
                return NULL;
            }else if(cmd_prev->input != NULL || cmd_prev->output != NULL)
                return NULL:
            else{   //Check to see if there is a SIMPLE_COMMAND after redirect
                current_node = next_token(current_node);
                if(current_node->type == WORD){
                    cmd_prev->input = current_node->token;  //The input for left redir is the word of current node
                    break;
                }
                else{
                    fprintf(stderr, "\nThere is no word after the left redirect.\n");
                    return NULL;
                }
            }
        }
        
        //////////////////
        //RIGHT REDIRECT//
        //////////////////
        if(current_node->token_type == RIGHT_REDIRECT){
            if(cmd_prev->output != NULL){
                fprintf(stderr, "\nRight redirect has output field already filled.\n");
                return NULL;
            }
            if(cmd_prev->type != SIMPLE_COMMAND || cmd_prev->type != SUBSHELL_COMMAND){ //simple command = word
                fprintf(stderr, "\nFormatting issue with right redirect.\n");
                return NULL;
            }
            current_node = next_token(current_node);
            if(current_node->type == WORD){      //Check for SIMPLE_COMMAND
                cmd_prev->output = current_node->token;
                break;
            }
            //At this point we've reached a formatting error
            fprintf(stderr, "\nFormatting issue with right redirect. No SIMPLE_COMMAND after?\n");
        }
        
        ///////////////// Same precedence as OR
        ///////AND///////
        /////////////////
        if(current_node->token_type == AND){
            
        }
        
        ////////////////// Same precedence as AND
        ////////OR///////
        /////////////////
        if(current_node->token_type == OR){
            
        }
        
        /////////////////
        //////WORD///////
        /////////////////
        if(current_node->token_type == WORD){
            
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
                           
///////////////////////////////////////////////////////////////////
//Create the appearance of a stack in order to hold the commands.//
///////////////////////////////////////////////////////////////////

struct stack{
  int num_contents;
  int max_contents;
  void** contents;
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


///////////////////////////////////////////////////////////////////
//Creates the stream of tokens for use in stack processing later.//
///////////////////////////////////////////////////////////////////
struct token_node_list* create_token_stream(char* input, int num_of_chars){
    //Create the token node list
    struct token_node_list* new_token_list= malloc(sizeof(struct token_node_list));
    struct token_node_list* list_iterator = new_token_list;
    list_iterator->head = new_token_list;


    
    char char_to_sort = *input;
    //char next_char = *input;
    
    int char_num_counter = 0;
    while(char_num_counter < num_of_chars){
        //Check to see if word
        if(isWord(char_to_sort)){
            //If so, store the word in its own token
            int word_index = 0;
            char* w = malloc(sizeof(char));
            if( w == NULL)
                fprintf(stderr, "\n Error allocating memory for word.");
            
            while(isWord(char_to_sort)){
                w[word_index] = char_to_sort;
                word_index++;
                
                w = realloc(w, (word_index+1)*sizeof(char));
                if(w == NULL)
                    fprintf(stderr, "\n Error allocating memory for word.");

                char_num_counter++;     //increment index
                input++;                //increment stream pointer
                char_to_sort = *input;
                
                if(char_num_counter ==  num_of_chars)
                    break;
            }
            list_iterator->next = add_token(new_token_list, w, WORD);
            list_iterator = list_iterator->next;
        }
        //Check for subshell
        else if( char_to_sort == '('){
          //TODO
            int num_pairs = 1;
            int open_pars = 1;
            int close_pars = 0;
            int parens_valid = 0;
            
            int index = 0;
            int buf_size = 10;      //random low num for most purposes
            
            char *ss_buf = malloc(buf_size* sizeof(char));
            if(ss_buf == NULL)
                fprintf(stderr, "\n Error allocating memory for subshell parse.\n");
            
            while(num_pairs>0){
                char_num_counter++;     //increment index
                input++;                //increment stream pointer
                char_to_sort = *input;
                if(char_num_counter == num_of_chars) //gone through all chars
                    return NULL;
                else if(char_to_sort == '('){
                    open_pars++;
                    num_pairs++;
                }
                else if(char_to_sort == ')'){
                    close_pars++;
                    num_pairs--;
                    parens_valid = open_pars-close_pars;
                    if( num_pairs == 0 && parens_valid == 0)
                        break;
                    else if(num_pairs == 0 && parens_valid != 0){
                        fprintf(stderr, "\n Mismatched parentheses.\n");
                        return NULL;
                    }
                        
                }
                else if(char_to_sort == '\n'){
                    while(1){                        //Eliminate useless characters
                        if(input[1] == ' ' || input[1] == '\v' || input[1] == '\r' || input[1] == '\t' || input[1] == '\n')
                            break;
                        input++;
                        char_to_sort++;
                    }
                    //Spec says to substitute semicolon for \n
                    char_to_sort = ';';
                }
                ss_buf[index] = char_to_sort;
                index++;
                
                if(index == buf_size){
                    buf_size+=2;
                    ss_buf = realloc(ss_buf, buf_size*sizeof(char));
                    if(ss_buf == NULL){
                        fprintf(stderr, "\n Error reallocating memory for subshell buffer.\n");
                        return NULL;
                    }
                }
                list_iterator->next = add_token(new_token_list, ss_buf, SUBSHELL);
                list_iterator = list_iterator->next;
            }
            
        }
        //Check for OR and PIPE
        else if(char_to_sort == '|'){
            char_num_counter++;
            input++; //increment pointer
            char_to_sort = *input;
            //Check to see if the next character is also a pipe, this is an OR
            if(char_to_sort == '|'){
                //Add a token node for OR
                list_iterator->next = add_token(new_token_list, char_to_sort, OR);
                list_iterator = list_iterator->next;
            }else if(char_to_sort != '|'){ //PIPE
                list_iterator->next = add_token(new_token_list, char_to_sort, PIPE);
                list_iterator = list_iterator->next;
            }
        }
        
        //Check for & and AND, code is same as OR case
        else if(char_to_sort == '&'){
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
            
            if(char_to_sort == '&'){       //This is an and
                list_iterator->next = add_token(new_token_list, char_to_sort, AND);
                list_iterator = list_iterator->next;
            }else if(char_to_sort != '&'){
                return NULL;
            }
        }
        
        //Check for left redirect
        else if(char_to_sort == '<'){
            list_iterator->next = add_token(new_token_list, char_to_sort, LEFT_REDIRECT);
            list_iterator = list_iterator->next;
            
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
        }
        //Check for right redirect
        else if(char_to_sort == '>'){
            list_iterator->next = add_token(new_token_list, char_to_sort, RIGHT_REDIRECT);
            list_iterator = list_iterator->next;
            
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
        }
        
        
        //Check for semicolon
        else if(char_to_sort == ';'){
            list_iterator->next = add_token(new_token_list, char_to_sort, SEMICOLON);
            list_iterator = list_iterator->next;
            
            char_num_counter++;     //increment index
            input++;                //increment stream pointer
            char_to_sort = *input;
            
        }else{
            fprintf(stderr,"\nCharacter is not a word or a special token.\n");
            return NULL;    //no character matches
        }
    }
    //Return pointer to the top of the token_stream
    return new_token_list;
}
                           


                           
command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
    //Take the full stream of chars in the command line and place them in a buffer
    int buffer_scalar = 20; //Good enough for 20 chars
    char* input_stream = malloc(buffer_scalar * sizeof(char));
    if(input_stream == NULL)
        fprintf(stderr, "Failed to allocate memory for input_stream. \n");
    
    char next_input_char = ' ';
    long int char_num = 0;
    
    while(1){
        next_input_char = getbyte(arg);
        if(next_input_char == -1 || next_input_char == EOF)
            break;
        
        if(next_input_char == '#'){ // this is a comment, skip until EOF or newline
            while((next_input_char != EOF) && (next_input_char != '\n') && (next_input_char != -1)){
                next_input_char = getbyte(arg);
            }
        }
        
        //Start loading stuff into input_stream buffer
        input_stream[char_num] = next_input_char;
        //We need to add a char, so increment the input_size counter
        char_num++;
        
        //Our small buffer needs to be reallocated every time now
        if(char_num > 20){
            input_stream = realloc(input_stream, (char_num)*sizeof(char));
            if(input_stream == NULL)
                fprintf(stderr, "Failed to reallocate memory for input_stream. \n");
        }
    }
    //Check to see if nothing was pulled from command line
    if(char_num == 0){
        free(input_stream);
        return NULL;
    }
    
    //If we've reached here a buffer has been created
    //Make streams
    struct token_node_list* stream = create_token_stream(input_stream, char_num);
    
    if(stream != NULL){
        //Create forest
    }
    
    free(input_stream)
    //Deal with freeing the linked list
    return command_stream;
}


command_t
read_command_stream (command_stream_t s)
{
    if(s == NULL || s->cur_node == NULL){
        fprintf(stderr, "\nCommand stream in read command is NULL.\n");
        return NULL;
    }
    struct cmd_node *cmd_node_print = s->cur_node;
    s->cur_node = s->cur_node->next_node;
    
    return cmd_node_print->cmd;
}
