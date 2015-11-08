// UCLA CS 111 Lab 1 command interface

typedef struct command *command_t;
typedef struct command_stream *command_stream_t;

typedef struct graphnode *graphnode_t;
struct graphnode{
  char *filename;
  graphnode_t next_node;
};

graphnode_t create_dependency_graph(command_t c);
int find_dependencies(graphnode_t list1, graphnode_t list2);


struct command_stream{
  command_t command;
  struct command_stream *next;
  struct cmd_node *head;
  struct cmd_node *cur_node;
  struct cmd_node *tail;
}
/* Create a command stream from LABEL, GETBYTE, and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the integer flag is
   nonzero.  */
void execute_command (command_t, int);

void execute_AND_command(command_t);
void execute_SEQUENCE_command(command_t);
void execute_PIPE_command(command_t);
void execute_simple_command(command_t);
void execute_combined_io(command_t);

void IO_error_check(int, command_t, int);
void IO_redirect(command_t);

void switch_on_command(command_t);
/* Return the exit status of a command, which must have previously been executed.
   Wait for the command, if it is not already finished.  */
int command_status (command_t);
