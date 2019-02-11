#include <stdbool.h>
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
	OUT,
	ERR
} direction;

// @func cmd
// @brief executes a command in the foreground
// @param tokens array of tokens for parsing
// @param args1 array to fill with args
// @param args2 array to fill with args
// @return exit code of child process
int cmd(char *tokens[], char *args1[], char *args2[], bool bg);

