// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stddef.h>

#include <error.h>
#include <unistd.h> //Access dup system call
#include <stdio.h> 
#include <string.h>
#include "alloc.h"

#define GEN_ERR  1
#define FILE_ERR 2
#define EXEC_ERR 3
#define PIPE_ERR 4
#define PROC_ERR 5


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
  int p_status;
  pid_t cp = fork();
  int dum = time_travel;
  if(cp == 0){
    cp = fork();
    if(cp == 0){ //grandchild
      execute_command(c->u.command[0], time_travel);
      exit(0);
    }else if(cp > 0){
      wait(cp, &p_status, 0);
      execute_command(c->u.command[1], time_travel);
      exit(0);
    }else
         error(PROC_ERR, 0, "Error with child process in exec_SEQUENCE \n");

  }else if(cp > 0){
    waitpid(cp, &p_status, 0);
    c->status = p_status;
    execute_command(c->u.command[1], time_travel);
   }else
     error(PROC_ERR, 0, "Error with child process in exec_SEQUENCE \n");
 

}

void exec_OR(command_t c, int time_travel){
  int dum = time_travel;
  execute_command(c->u.command[0], time_travel);
  c->status = c->u.command[0]->status;

  //  int cmd_success = -1;
  //cmd_success = c->status;
   //0 if good
  //For OR, we need 1 of the two to be successful
  if(c->status){
    execute_command(c->u.command[1], time_travel);
    c->status = c->u.command[1]->status;
  }
}

void exec_PIPE(command_t c, int time_travel){
  int dum = time_travel;
  pid_t new_p;
  //do pipe things
  int buf[2];
  if(pipe(buf) == -1)
    error(PIPE_ERR, 0, "Error creating pipe.\n");
  new_p = fork();
  if (new_p == 0) {
  	//close end of buffer and start the first command
  	close(buf[0]);
  	if(dup2(buf[1], 1) == -1)
    	    error(FILE_ERR, 0, "dup2 error in exec_pipe 1st command.\n");
  	execute_command(c->u.command[0], time_travel);
  	c->status = c->u.command[0]->status;

  	close(buf[1]);
	exit(0);
  } else if (new_p > 0) {
    int p_status;
    waitpid(new_p, &p_status, 0);
    close(buf[1]);
  	if(dup2(buf[0], 0) == -1)
    	    error(FILE_ERR, 0, "dup2 error in exec_pipe 2nd command.\n");
  	execute_command(c->u.command[1], time_travel);
  	c->status = c->u.command[1]->status;
  	close(buf[0]);
  } else {
	error(PIPE_ERR, 0, "Cannot create pipe.\n");
  }
}

void exec_SIMPLE(command_t c, int time_travel){
  int file_status;
  int dup_success = -1;
  int file = 0;
  int file_out = 0;
  int dum = time_travel;
  pid_t cp;  
  //  int dummy = time_travel;
  //Check to see if IO is null or not
  cp = fork();
  if(cp == 0){
  if(c->input != NULL){
    file = open(c->input, O_RDWR);
    if(file == -1 )
      error(FILE_ERR, 0, "file error in exec_simple input.\n");

    dup_success = dup2(file, 0);
    if(dup_success == -1)
      error(FILE_ERR, 0, "error with dup2 in exec_simple input.\n");
    close(file);
  }
  if(c->output != NULL){
    file_out = open(c->output, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(file_out == -1)
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

  else if(cp > 0){
    int p_status;
    waitpid(cp, &p_status, 0);
    c->status = p_status;
  }else
    error(PROC_ERR, 0, "Error with child process in exec_SIMPLE. \n");
}

void exec_SUBSHELL(command_t c, int time_travel){
  int dum = time_travel;
  execute_command(c->u.subshell_command, time_travel);
  c->status = c->u.subshell_command->status;
}

void
execute_command (command_t c, int time_travel)
{
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
