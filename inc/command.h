// File: command.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains function prototypes for command.c

// direction of file redirection
enum direction {
	IN,
	OUT
};


// @func cmd
// @brief executes a command in its own process
// @param args arguments to be passed to exec (null terminated)
// @return exit code of child process
int cmd(char *args[]);

// @func redirect
// @brief executes a command, redirecting output to a file
// @param args arguments to be passed to exec (null terminated)
// @param out filename to redirect input from/output to
// @param dir the direction of the redirection
// @return exit code of child process
//int redirect(char *args[], char *out, direction dir); 

// @func pipeDreams
// @brief see HEAT by BROCKHAMPTON
// (it actually pipes two commands together)
// @param args1
// @param args2
// @return exit code of commands
int pipeDreams(char *args1[], char *args2[]);
