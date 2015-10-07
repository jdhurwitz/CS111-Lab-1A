// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include <stdio.h>
#include <ctype.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>
#include "alloc.h"

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

//Structs (nodes) for tokens and commands
struct cmd_node{
  struct command *cmd;  //This is how command is accessed
  struct cmd_node *next_node;
};

struct command_stream{
  command_t command;
  struct command_stream *next;
  struct cmd_node *head;
  struct cmd_node *cur_node;
  struct cmd_node *tail;
};


enum token_name{
    DUMMY_HEAD,
    SUBSHELL,
    LEFT_REDIRECT,
    RIGHT_REDIRECT,
    AND,
    OR,
    PIPE,
    SEMICOLON,
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

///////////////////////////////////////////////////////////////////
//Create the appearance of a stack in order to hold the commands.//
///////////////////////////////////////////////////////////////////
struct stack{
    int num_contents;
    int max_contents;
    command_t contents[10];
};

void remove_stack (struct stack *stack) {
    int i;
    for (i = 0; i < stack->num_contents; i++) {
        free (stack->contents[i]);
    }
    free (stack->contents);
    free (stack);
    return;
}

void push (struct stack *stack, command_t val, int increase_size) {
    /*
    if (stack->num_contents == stack->max_contents) {
        stack->max_contents = stack->max_contents + increase_size;
        stack->contents = realloc (stack->contents, stack->max_contents * sizeof(void));
    }
     */
    stack->contents[stack->num_contents] = val;
    stack->num_contents = stack->num_contents + 1;
    return;
}


command_t view_top (struct stack *stack) {
    if (stack->num_contents == 0) {
        return NULL;
    } else {
        command_t top_val = stack->contents[stack->num_contents-1];
        return top_val;
    }
}

command_t pop(struct stack *user_stack){
    if(user_stack->num_contents <= 0){
        return NULL;
    }
    else{
        int new_num_contents = user_stack->num_contents -1;
        user_stack->num_contents = new_num_contents;
        command_t modified_stack = user_stack->contents[user_stack->num_contents];
        return modified_stack;
    }
    return NULL;
    
}
/*

struct stack* create_stack(){
    int stack_size = sizeof(struct stack);
    struct stack* new_stack = malloc(stack_size);
    new_stack->num_contents = 0; //Nothing in the stack atm
    
    int contents_alloc_size = new_stack->max_contents * sizeof(void*);
    new_stack->contents = malloc(contents_alloc_size);
    return new_stack;
}

*/


struct token_node *next_token(struct token_node *token){
  //Make sure it isn't the tail or head node in single node case
  if(token == NULL)
    error(1,0, "Error in next_token, token ptr NULL.");

  if(token->next_node != NULL)
    return token->next_node;
  else
    return NULL;
}

struct command_stream *next_command_stream(struct command_stream *cmd_stream){
    //Make sure it isn't the tail or head node in single node case
    if(cmd_stream == NULL)
        error(1,0, "Error in next_token, token ptr NULL.");
    
    if(cmd_stream->next != NULL)
        return cmd_stream->next;
    else
        return NULL;
}

struct token_node *add_token(struct token_node_list *tokens, char *token_to_add, enum token_name type){
  int token_node_size = sizeof(struct token_node);
  struct token_node * new_token = malloc(token_node_size);
  new_token->token_type = type;
  //Check to see if the list is empty, and add if it is
  if(tokens->head == NULL){
    tokens->head = new_token;
    //Since there is no next node, take care of next pointer
    tokens->head->next_node = NULL;
    tokens->head->token = token_to_add;
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

 
struct cmd_node * add_cmd_node (struct command_stream * c_list, struct command * n_command) {
  int node_size = sizeof(struct cmd_node);

  if (c_list->head == NULL){
    struct cmd_node * n_node = malloc (node_size);
    c_list->head = n_node;
    c_list->head->next_node = NULL;
    c_list->head->cmd = n_command;
    c_list->cur_node = c_list->head;
    c_list->tail = c_list->cur_node;
    return c_list->cur_node;

  } else {
    struct cmd_node * new_node = malloc(node_size);
    new_node->next_node = NULL;
    new_node->cmd = n_command;
    c_list->cur_node = c_list->cur_node->next_node;
    c_list->tail = c_list->cur_node;
    return c_list->cur_node;

  }

}



//Function to combine commands and an operator. RETURN 1 if success, 0 if fail
int combine(struct stack* operator_stack, struct stack* operand_stack){
    //if(operator_stack->num_contents  == 0) //No operators
      //  return 0;
    if(operand_stack->num_contents < 2) //not enough commands
        return 0;
    command_t combined = pop(operator_stack);
    /*
    combined->input = NULL;
    combined->output = NULL;
    */
    command_t second = pop(operand_stack);
    command_t first  = pop(operand_stack);
    
    combined->u.command[1] = second;
    combined->u.command[0] = first;
    push(operand_stack, combined, 10); //10 for realloc size
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




///////////////////////////////////////////////////////////////////
//Creates the stream of tokens for use in stack processing later.//
///////////////////////////////////////////////////////////////////
struct token_node_list* create_token_stream(char* input, int num_of_chars){
    
    //Create the token node list
    struct token_node_list* new_token_list= malloc(sizeof(struct token_node_list));
    struct token_node_list* head_of_list = new_token_list;
    //Make dummy token in order to avoid NULL token_type pointer
    struct token_node* dummy_head = add_token(new_token_list, NULL,DUMMY_HEAD);
    new_token_list->head = dummy_head;


    
    char char_to_sort = *input;
    int nested_breaker = 0;
    
 //char next_char = *input;
    int char_num_counter = 0;
    while(char_num_counter < num_of_chars){
        if(char_to_sort == '\000'){
            return head_of_list;
        }
        //Check to see if word
        if(isWord(char_to_sort)){
            //If so, store the word in its own token
            size_t size = 8;

            char* w = checked_malloc(size);
            size_t word_index = 0;

            /*
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
             */
            
            
            do{
                w[word_index] = char_to_sort;
                word_index++;
                
                if(word_index == size){
                    size = size*2;
                    w = checked_grow_alloc(w, &size);
                }
                char_num_counter++;     //increment index
                input++;                //increment stream pointer
                char_to_sort = *input;
                
            }while(isWord(char_to_sort) && char_num_counter < num_of_chars);
            new_token_list->cur_node = add_token(new_token_list, w, WORD);
            
        }
        
        //Handle useless white space
        else if(char_to_sort == ' ' || char_to_sort == '\t'){
            char_num_counter++;     //increment index
            input++;                //increment stream pointer
            char_to_sort = *input;
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
                if(char_num_counter == num_of_chars){ //gone through all chars
                    fprintf(stderr, "\nAll characters used.\n");
                    return NULL;
                }
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
                    while(input[1] != ' ' || input[1] != '\t' || input[1] != '\n'){                        //Eliminate useless characters
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
                new_token_list->cur_node = add_token(new_token_list, ss_buf, SUBSHELL);
                
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
                new_token_list->cur_node = add_token(new_token_list, NULL, OR);
                
                char_num_counter++;
                input++;                    //increment pointer
                char_to_sort = *input;         //peek at the next character

            }else
                new_token_list->cur_node = add_token(new_token_list, NULL, PIPE);

        }
        
        //Check for & and AND, code is same as OR case
        else if(char_to_sort == '&'){
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
            
            if(char_to_sort == '&'){       //This is an and
                new_token_list->cur_node = add_token(new_token_list, NULL, AND);
                
                char_num_counter++;
                input++;                    //increment pointer
                char_to_sort = *input;         //peek at the next character
            }else if(char_to_sort != '&'){
                fprintf(stderr, "\n Single and...error.\n");
                return NULL;
            }
        }
        
        //Check for left redirect
        else if(char_to_sort == '<'){
            new_token_list->cur_node = add_token(new_token_list, NULL, LEFT_REDIRECT);
            
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
        }
        //Check for right redirect
        else if(char_to_sort == '>'){
            new_token_list->cur_node = add_token(new_token_list, NULL, RIGHT_REDIRECT);

            
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
        }
        
        //Handle newline
        else if(char_to_sort == '\n'){
            if(*input++ == '\000'){
                return head_of_list;
                break;
            }
                
            switch(new_token_list->cur_node->token_type){
                case LEFT_REDIRECT:
                case RIGHT_REDIRECT:
                    fprintf(stderr, "\n Error in syntax. Redirect before newline.\n");
                    return NULL;
                    break;
                case WORD:
                case SUBSHELL:
                    if(new_token_list->cur_node->token_type != DUMMY_HEAD){
                        new_token_list->next = malloc(sizeof(struct token_node_list));
                        if(new_token_list->next == NULL){
                            fprintf(stderr, "\nError allocating memory for new tree in create_token_stream.\n");
                            return NULL;
                        }
                        
                        
                        new_token_list = new_token_list->next;
                        if(new_token_list == NULL)
                            fprintf(stderr, "\n new_token_list is NULL in handling newline\n");
                        
                        new_token_list->head = add_token(new_token_list, NULL, DUMMY_HEAD );
                        new_token_list->cur_node = new_token_list->head;

                    }
                    break;
                default:
                    break;
                    //if(new_token_list->cur_node->token_type == DUMMY_HEAD)
                      //  break;
                    //else
                    //return NULL;
            }
            char_num_counter++;
            input++;                    //increment pointer
            char_to_sort = *input;         //peek at the next character
        }
        //Check for semicolon
        else if(char_to_sort == ';'){
            new_token_list->cur_node = add_token(new_token_list, NULL, SEMICOLON);
            
            char_num_counter++;     //increment index
            input++;                //increment stream pointer
            char_to_sort = *input;
            
        }
        else{
            fprintf(stderr,"\nCharacter is not a word or a special token.\n");
            return NULL;    //no character matches
        }
    }
    //Return pointer to the top of the token_stream
    return head_of_list;
}


///////////////////////////////////////////////////////////////////
////Creates command tree to be used in making a command forest.////
///////////////////////////////////////////////////////////////////
command_t create_tree (struct token_node *token){
    //list->cur_node = list->head;
    if(token == NULL){
        fprintf(stderr, "\ntoken_node_list is NULL in create_tree\n");
    }
    command_t cmd = malloc(sizeof(struct command));
    command_t cmd_prev = NULL;
    struct token_node * current_node = token;
    
    //Create operator and command stack of size 50
    struct stack* operand_stack = malloc(sizeof(struct stack));//create_stack();
    struct stack* operator_stack = malloc(sizeof(struct stack));//create_stack();
    operand_stack->num_contents = 0;
    operator_stack->num_contents = 0;

        do{
            if( !(current_node->token_type == LEFT_REDIRECT || current_node->token_type == RIGHT_REDIRECT) )
            {
                // make new command
                cmd = checked_malloc(sizeof( struct command ));
            }
            /////////////////
            /////SUBSHELL////
            /////////////////
            if(current_node->token_type == SUBSHELL){
                cmd->type = SUBSHELL_COMMAND;
                int length_of_token = strlen(current_node->token);
                struct token_node_list *token_stream = create_token_stream(current_node->token,length_of_token);
                cmd->u.subshell_command = create_tree(token_stream->head);
                push(operand_stack, cmd,1);
            }
            
            /////////////////
            //LEFT REDIRECT//
            /////////////////
            else if(current_node->token_type == LEFT_REDIRECT){
                if(cmd_prev == NULL){
                    fprintf(stderr, "\n cmd_prev NULL .\n");
                    return NULL;
                }
                if(!(cmd_prev->type == SIMPLE_COMMAND || cmd_prev->type == SUBSHELL_COMMAND)){ //simple command = word
                    fprintf(stderr, "\nFormatting issue with left redirect.\n");
                    return NULL;
                }else if(cmd_prev->input != NULL || cmd_prev->output != NULL)
                    return NULL;
                else{   //Check to see if there is a SIMPLE_COMMAND after redirect
                    current_node = next_token(current_node);
                    if(current_node->token_type == WORD){
                        cmd_prev->input = current_node->token;  //The input for left redir is the word of current node
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
            else if(current_node->token_type == RIGHT_REDIRECT){
                if(cmd_prev == NULL){
                    fprintf(stderr, "\n cmd_prev NULL in left redirect.\n");
                    return NULL;
                }
                if(cmd_prev->output != NULL){
                    fprintf(stderr, "\nRight redirect has output field already filled.\n");
                    return NULL;
                }
                if(!(cmd_prev->type == SIMPLE_COMMAND || cmd_prev->type == SUBSHELL_COMMAND)){ //simple command = word
                    fprintf(stderr, "\nFormatting issue with right redirect.\n");
                    return NULL;
                }
                current_node = next_token(current_node);
                if(current_node->token_type == WORD){      //Check for SIMPLE_COMMAND
                    cmd_prev->output = current_node->token;
                }else{
                //At this point we've reached a formatting error
                fprintf(stderr, "\nFormatting issue with right redirect. No SIMPLE_COMMAND after?\n");
                }
            }
            
            ///////////////// Same precedence as OR
            ///////AND///////
            /////////////////
            else if(current_node->token_type == AND){
                cmd->type = AND_COMMAND;
                int precedence_check = 0;
                int empty = 0;
                if(operator_stack->num_contents == 0)
                    empty = 1;
                
                
                if(!empty && (view_top(operator_stack)->type == AND_COMMAND || view_top(operator_stack)->type == OR_COMMAND || view_top(operator_stack)->type == PIPE_COMMAND))
                    precedence_check = 1;
                if(precedence_check && (operator_stack->num_contents > 0)){
                    if(combine(operator_stack, operand_stack) == 0){
                        fprintf(stderr, "\n Either operand stack has fewer than 2 ops or operator stack is empty. (AND)\n");
                        return NULL;
                    }
                }
                push(operator_stack, cmd, 1);
                
            }
            ////////////////// Same precedence as AND
            ////////OR///////
            /////////////////
            else if(current_node->token_type == OR){
                cmd->type = OR_COMMAND;
                int precedence_check = 0;
                int empty = 0;
                if(operator_stack->num_contents == 0)
                    empty = 1;
                
                if(!empty && (view_top(operator_stack)->type == AND_COMMAND || view_top(operator_stack)->type == OR_COMMAND || view_top(operator_stack)->type == PIPE_COMMAND))
                    precedence_check = 1;
                if(precedence_check && (operator_stack->num_contents > 0)){
                    if(combine(operator_stack, operand_stack) == 0){
                        fprintf(stderr, "\n Either operand stack has fewer than 2 ops or operator stack is empty. (OR)\n");
                        return NULL;
                    }
                }
                push(operator_stack, cmd, 1);
            }

            
        ///////////////// Highest priority, so can avoid checking I/O, etc.
        //////PIPE///////
        /////////////////
        else if(current_node->token_type == PIPE){
            cmd->type = PIPE_COMMAND;
            if((operator_stack->num_contents > 0) && (view_top(operator_stack)->type == PIPE_COMMAND)){
                int combine_result = combine(operator_stack, operand_stack);
                if(combine_result == 0){
                    fprintf(stderr, "\nError combining commands for pipe. Stacks too small.\n");
                    return NULL;
                }
            }
            push(operator_stack, cmd, 1);
            
        }
        
        ///////////////// Lowest precedence, along with newline
        ////SEMICOLON////
        /////////////////
        else if(current_node->token_type == SEMICOLON){
            cmd->type = SEQUENCE_COMMAND;
            if(combine(operator_stack, operand_stack) == 1)
                push(operator_stack,cmd ,10);
            else{
                fprintf(stderr, "\n Either operand stack has fewer than 2 ops or operator stack is empty. (SEMICOLON)\n");
                return NULL;
            }
                
        }
       
        /////////////////
        //////WORD///////
        /////////////////
        else if(current_node->token_type == WORD){
            cmd->type = SIMPLE_COMMAND;
            //At leat one word exists
            int words = 1;
            int alloc_size = 0;
            int index = 1;
            int set_word_success = 1;
            
            struct token_node *token_iterator = current_node;
            while(1){
                if(token_iterator->next_node == NULL)
                    break;
                if(token_iterator->next_node->token_type != WORD)
                    break;
                token_iterator = next_token(token_iterator);
                words++;
            }
            alloc_size = (words+1)*sizeof(char*);
            if(alloc_size != 0){
                cmd->u.word = malloc(alloc_size);
                if(cmd->u.word == NULL)
                    fprintf(stderr, "\n Error allocating memory for word in create_tree.\n");
            }
            cmd->u.word[0] = current_node->token;
            while(index < words){
                current_node = next_token(current_node);
                cmd->u.word[index] = current_node->token;
                index++;
                if(index == words)
                    set_word_success = 1;
            }
            //Check to see if all were set successfully
            cmd->u.word[words] = NULL;
            push(operand_stack, cmd, 1);
        }
      cmd_prev = cmd;
    } while(current_node != NULL && (current_node = current_node->next_node) != NULL);
    
    int combine_check = 0;
    while(operator_stack->num_contents >0){
	combine_check = combine(operator_stack, operand_stack);
        if(combine_check == 0){
            fprintf(stderr, "\nEither not enough operands or operators in stack at end of create_tree\n");
            return NULL;
        }
    }
	if(operand_stack->num_contents != 1){
          fprintf(stderr, "\n Command stack != 1 items in create_tree.\n");
          return NULL;
	}
	//
	command_t final_tree = pop(operand_stack);
	free(operand_stack);
	free(operator_stack);
	return final_tree;
    

}





command_stream_t make_forest (struct token_node_list *list) {
    struct command_stream* current_tree = NULL;
    struct command_stream* previous_tree = NULL;
    struct command_stream* head_tree = NULL;
    struct command_stream* tail_tree = NULL;
    
    while (list != NULL && list->head->next_node != NULL && 1) {
        struct token_node* current = list->head->next_node;
        //Take the token stream and convert it to a tree.
        command_t command_node = create_tree(current);
        current_tree = malloc(sizeof(struct command_stream));
        if (current_tree == NULL){
            fprintf(stderr, "Was not able to allocate memory to standard error");
        }
        
        //Test
        current_tree->command = command_node;
        
        
        //The first iteration, so set both head and previous tree to current_tree (the first one)
        if (!head_tree) {
             head_tree = current_tree;
             previous_tree = head_tree;
             //head_tree = current_tree;
             //A later iteration, so increment the previous_tree while keeping head_tree constant.
        } else {
            /*
             tail_tree = head_tree;
             previous_tree->next = current_tree;
             tail_tree = NULL;
             previous_tree = current_tree;
             tail_tree = previous_tree->next;
             */
            previous_tree->next = current_tree;
            previous_tree = current_tree;
        }
            list = list->next;
            tail_tree = NULL;
        }
    //Return the head_tree which should be the first tree linked to the rest of the trees.
        return head_tree;
}
                    

command_t encounter_operator(struct stack *operator_stack, struct stack *cmd_stack){
    //operator stack and command stack as parameters
              command_t op = pop(operator_stack);
              command_t cmd1, cmd2;
              command_t combined_cmd = NULL;
              //Look for two commands and consolidate for precedence issues later.
              //Second command will be on the top of the stack
              int s = 1;
              switch(s){
                  case 1:
                      break;
                  case '<':
                  case '>':
                  case '\n':
                  case ';':
                  case '|':
                      cmd2 = pop(cmd_stack);
                      cmd1 = pop(cmd_stack);
                      break;
                  case '(':
                  case ')':
                      cmd1 = pop(cmd_stack);
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
              while( (token_iterator = next_token(prev_cur))){
                  fprintf(stderr, "<%s>\n", token_iterator->token);
              }
              t->cur_node = prev_cur;
              return;
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
        next_input_char = get_next_byte(get_next_byte_argument);
        if(next_input_char == -1 || next_input_char == EOF)
            break;
        
        if(next_input_char == '#'){ // this is a comment, skip until EOF or newline
            while((next_input_char != EOF) && (next_input_char != '\n') && (next_input_char != -1)){
                next_input_char = get_next_byte(get_next_byte_argument);
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
    if (stream == NULL) {
        fprintf(stderr, "\n stream is null");
        return NULL;
    }
    command_stream_t cs = make_forest(stream);
    if (cs == NULL) {
        fprintf(stderr, "\n command stream is null after command forest created \n");
        return NULL;
    }
    
    free(input_stream);
    //Deal with freeing the linked list
    return cs;
}


command_t
read_command_stream (command_stream_t s)
{
    if(s == NULL || s->command == NULL){
        fprintf(stderr, "\nCommand stream in read command is NULL.\n");
        return NULL;
    }
    command_t cmd_return= s->command;
    
    if(s->next != NULL){
        command_stream_t next = s->next;
        s->command = s->next->command;
        s->next = s->next->next;
    }else
        s->command = NULL;
    
    return(cmd_return);
}
