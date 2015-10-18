// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <error.h>
#include <unistd.h> //Access dup system call
#include <stdio.h> 

#include "alloc.h"

#define GEN_ERR  1
#define FILE_ERR 2
#define EXEC_ERR 3
#define PIPE_ERR 4

int
command_status (command_t c)
{
  return c->status;
}

void exec_AND(command_t c, int time_travel){
  int dum = time_travel;
  execute_command(c->u.command[0], time_travel);
  c->status = c->u.command[0]->status;

  int cmd_completion = -1;
  cmd_completion = c->u.command[0]->status;
  if(cmd_completion == 0){ //success
    execute_command(c->u.command[1], time_travel);
    c->status = c->u.command[1]->status;
  } 
  
}

void exec_SEQUENCE(command_t c, int time_travel){
  int dum = time_travel;
  execute_command(c->u.command[0], time_travel);
  execute_command(c->u.command[1], time_travel);
  c->status = c->u.command[1]->status;

}

void exec_OR(command_t c, int time_travel){
  int dum = time_travel;
  execute_command(c->u.command[0], time_travel);
  c->status = c->u.command[0]->status;

  int cmd_success = -1;
  cmd_success = c->u.command[0]->status; //0 if good
  //For OR, we need 1 of the two to be successful
  if(!(cmd_success )){
    execute_command(c->u.command[1], time_travel);
    c->status = c->u.command[1]->status;
  }
}

void exec_PIPE(command_t c, int time_travel){
  int dum = time_travel;
  //do pipe things
  int buf[2];
  if(pipe(buf) == -1)
    error(PIPE_ERR, 0, "Error creating pipe.\n");

  //close end of buffer and start the first command
  close(buf[0]);
  if(dup2(buf[1], 1) == -1)
    error(FILE_ERR, 0, "dup2 error in exec_pipe 1st command.\n");
  execute_command(c->u.command[0], time_travel);
  c->status = c->u.command[0]->status;

  close(buf[1]);
  if(dup2(buf[0], 0) == -1)
    error(FILE_ERR, 0, "dup2 error in exec_pipe 2nd command.\n");
  execute_command(c->u.command[1], time_travel);
  c->status = c->u.command[1]->status;
  close(buf[0]);
}

void exec_SIMPLE(command_t c, int time_travel){
  int file_status;
  int dup_success = -1;
  int file = 0;
  int file_out = 0;
  int dum = time_travel;
  
  //  int dummy = time_travel;
  //Check to see if IO is null or not
  if(c->input != NULL){
    file_status = open(c->input, O_RDONLY);
    if(file_status == -1 )
      error(FILE_ERR, 0, "file error in exec_simple input.\n");

    dup_success = dup2(file, 0);
    if(dup_success == -1)
      error(FILE_ERR, 0, "error with dup2 in exec_simple input.\n");
    close(file);
  }
  if(c->output != NULL){
    file_status = open(c->output, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(file_status == -1)
      error(FILE_ERR, 0, "error in exec_simple output.\n");

    dup_success = dup2(file_out, 1);
    if(dup_success == -1)
      error(FILE_ERR, 0, "error with dup2 in exec_simple output.\n");

    close(file_out);

  }
  execvp(c->u.word[0], c->u.word);
  //returns to this point if execvp fails
  error(EXEC_ERR, 0, "execvp error in exec_simple. \n");
}

void exec_SUBSHELL(command_t c, int time_travel){
  int dum = time_travel;
  execute_command(c->u.subshell_command, time_travel);
  //  c->status = c->u.subshell_command->status;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  int dummy = c->status;
  int dumm2 = time_travel;
  //  error (1, 0, "command execution not yet implemented");
  switch(c->type){
    case AND_COMMAND:
    {
      exec_AND(c, time_travel);
      break;
    }
    case SEQUENCE_COMMAND:
      {
	exec_SEQUENCE(c, time_travel);
	break;
      }
    case OR_COMMAND:
      {
	exec_OR(c, time_travel);
	break;
      }
    case PIPE_COMMAND:
      {
	exec_PIPE(c, time_travel);
	break;
      }
    case SIMPLE_COMMAND:
      {
	exec_SIMPLE(c, time_travel);
	break;
      }
    case SUBSHELL_COMMAND:
      {
	exec_SUBSHELL(c, time_travel);
	break;
      }
    default:
      error(1, 0, "No matching command types.\n");

  }


}

/*

void
IO_redirect(command_t c)
{

  if(c->input != NULL){
    int read_input = open(c->input, O_RDWR); //Read and write permission
    IO_error_check(read_input, c, 0);
  }

  if(c->output != NULL){
    int output = open(c->output, O_TRUNC | O_CREAT | S_IWUSR | S_IWGRP | O_WRONLY | S_IRUSR | S_IRGRP);
    IO_error_check(output, c, 1);
  } 
}


*/
