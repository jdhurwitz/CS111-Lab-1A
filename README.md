Specification notes for implementation of lab 1a.

Written by Jonathan Hurwitz and Ram Sivasundaram.

We used command forests which support command trees as subsets. We used tokenization and created token nodes as a further subset of the command trees. 

Function explanations:

isWord
	-Checks to see if a character is a word

is_operator 
	-Checks to see if a test character is an operator


/////
Stack
/////
We created the appearance of a stack with push and pop functions, as well as the ability to view the top of the stack without actually removing anything. This was all implemented by using structs that we called "stack."

The push and pop functions behave as they would in the C++ STL.
Pop returns a command_t from whatever stack is passed in.

next_token
	-Returns a pointer to the next node in a series of tokens
	-Used so that we wouldn't have to keep doing token = token->next_node

next_command_stream
	-Same as above but for type command_stream

add_token
	-Adds a token. If it's the first, it links it to the head of a
	token_node_list. If it is not the first then it is the next one following whatever is at the tail.

add_cmd_node
	-Adds a command node to a list of type command_stream

combine
	-This function is used to pop two operands off of the operand stack
	and pop one operator off of the operator stack. It links these as a combined
	command and then pops them back onto the operand_stack.
	-Used for nested commands.

create_subshell_cmd
	-A function we created to make a subshell command.
	(didn't end up using this)

///////////////////
create_token_stream
///////////////////
-Creates a token node list called new_token_list. The tokens will all point to this one.
-New token node lists will be created if necessary and linked to this one.

Note: Since this is a very long function I will provide a high level view of it:
      -tokenize the following: (, ), |, ||, &&, <, >, ;, \n
      -Eliminates useless things like white space and tabs

///////////
create_tree
///////////
-Makes a command tree and takes in a pointer to a token as the argument.
-Creates an operand stack and an operator stack

///////////
make_forest
///////////
-This function makes a command forest from the command trees

We make the command tree then traverse the token_node_lists in order to make the command forest that will serve as the final functional model for printing out.
----


Checking for Errors
-------------------
We created error codes and specific messages to help us determine when and why something fails. The codes are as follows:
1: generic error
2: syntax error
3: formatting error
4: unknown character
5: stack size or stack formatting error
6: pointer out of bounds error

We called the function error in order to handle these messages. Parameter 0 of this function will also exit the program and if its value is non-zero.


Dependencies were checked for recursively and when commands have dependencies, they're added into a list of command_streams that will be executed after all of the parallel code is executed. This way, we don't have to worry about order and overhead in blocking since the code will just execute everything in parallel first, and then execute all the dependent things serially. 

The dependency graph is built in make_forest.


Known Limitations
-----------------
Our program passed all of the test cases for test-p-ok.sh and test-p-bad.sh. We were not aware of any limitations.
