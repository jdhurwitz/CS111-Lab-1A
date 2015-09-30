// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <error.h>
#include <unistd.h> //Access dup system call
#include <stdio.h> 

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  int dummy = c->status;
  int dumm2 = time_travel;
  error (1, 0, "command execution not yet implemented");

}


void 
execute_AND_command(command_t c)
{
  switch_on_command(c->u.command[0]);
  //Check exit status (0 means hasn't exited). And requires both true
  if(c->u.command[0]->status == 0){
    switch_on_command(c->u.command[1]);
    c->status = c->u.command[1]->status;
  }
  else{
    c->status = c->u.command[0]->status;
  }
}


void
execute_OR_command(command_t c)
{
  switch_on_command(c->u.command[0]);
  if(c->u.command[0]->status != 0){
    switch_on_command(c->u.command[1]);
    c->status = c->u.command[1]->status;
  }
  else{
    c->status = c->u.command[0]->status;
  }
}

//FIXME
void
execute_SEQUENCE_command(command_t c){
  switch_on_command(c->u.command[0]);
  _exit(c->u.command[0]->status);

  switch_on_command(c->u.command[1]);
  _exit(c->u.command[1]->status);
}


//FIXME
void
execute_PIPE_command(command_t c){
  int grass[2]; //420
  pipe(grass);

  //2nd element is opened for writing
  if(dup2(grass[1],1) < 0){ //Error (stdout)
    error(1,0,"dup2 error in execute_PIPE_command");
  }
  close(grass[0]);
  switch_on_command(c->u.command[0]);
  
  if(dup2(grass[0],0) < 0){ //Error (stdin)
    error(1,0,"du2p error in execute_PIPE_command");
  }
  close(grass[1]);
  switch_on_command(c->u.command[1]);
  
}

void 
execute_simple_command(command_t c)
{
  IO_redirect(c);
  /*
    execvp() sys call:
    1) first arg is char string contains nam eof file to be exec
    2) second arg is pointer to an array of char strings (word)
   */
  execvp(c->u.word[0], c->u.word);
  //Will break, throw error if not..
  error(1,0, "Simple command failure in execute_simple_command");
}

/*
Combine calls for simple commands, subshells, and commands into one function.
 */
void
execute_combined_IO(command_t c){
  if(c->type == SUBSHELL_COMMAND){
    IO_redirect(c);
    switch_on_command(c->u.subshell_command); 
  }
  else if(c->type == SIMPLE_COMMAND){
    execute_simple_command(c);
  }
  else{ //Throw error
    error(1,0, "Error in execute_combined_IO");
    
  }
}

void
IO_error_check(int opened, command_t c, int isOutput){
  if(opened < 0){
    error(1,0, "Error returned from open function in IO_error_check: %s", c->input);
  }
  if(dup2(opened, isOutput) < 0) {
    error(1,0, "Error with sys. call dup2 in IO_error_check.");
  }
  if(close(opened) < 0) {
    error(1,0, "Error on closing file in IO_error check.");
  }
}

void
IO_redirect(command_t c)
{
  /*
  Check to see if input pointer is null, if not can do file operations.
  Pull from the file.
   */
  if(c->input != NULL){
    int read_input = open(c->input, O_RDWR); //Read and write permission
    IO_error_check(read_input, c, 0);
  }

  /*
  Check to see if the output pointer is null, if not, can copy all the 
  stuff from the input into the output after redirect.
   */
  if(c->output != NULL){
    int output = open(c->output, O_TRUNC | O_CREAT | S_IWUSR | S_IWGRP | O_WRONLY | S_IRUSR | S_IRGRP);
    IO_error_check(output, c, 1);
  } 
}


/*
Switch on the command that was entered. Call the respective helper
function to do actual execution.
 */
void 
switch_on_command(command_t c)
{
  switch(c->type){
    case AND_COMMAND:
      execute_AND_command(c);  
      break;
    case SEQUENCE_COMMAND:
      execute_SEQUENCE_command(c);
      break;
    case OR_COMMAND:
      execute_OR_command(c);
      break;
    case PIPE_COMMAND:
      execute_PIPE_command(c);
      break;
    case SIMPLE_COMMAND:
      execute_combined_io(c); //execute_combined calls execute_simple
      break;
    case SUBSHELL_COMMAND:
      execute_combined_io(c);
      break;
   default:
   //Output to stderr with a syntax error message that has line number and colon, then exit
      fprintf(stderr, "%s\n", "Error in switch_on_command; no enumerated type selected.");
      break;
  }
}
