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

struct token_node *add_token(struct token_node_list *tokens, char *token_to_add){
  //Check to see if the list is empty
  if(tokens->head_node == NULL){
    
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
