// File: command.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains function prototypes for command.c

// pipe end definitions
#define PIPE_RD		0
#define PIPE_WR		1

// direction of file redirection
typedef enum direction {
	IN,
	OUT
} direction;

// @func cmd
// @brief executes a command in the foreground
// @param args arguments to be passed to exec (null terminated)
// @return exit code of child process
int cmd(char *tokens[], char *args[]);

// @func pipeDreams
// @brief see HEAT by BROCKHAMPTON
// (it actually pipes two commands together)
// @param args1
// @param args2
// @return -1 for failure, 0 otherwise
int pipeDreams(char *args1[], char *args2[]);
