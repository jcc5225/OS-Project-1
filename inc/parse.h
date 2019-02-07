// File: parse.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains function prototypes for parse.c

#define TOKEN_LEN	31
#define TOKENS_SIZE	67
#define ARGS_SIZE	67

// @func getArgs
// @brief extracts command arguments from an array of user-received tokens
// @param tokens array of tokens
// @param args array of arguments to fill
// @return void
void getArgs(char *tokens[], char *args[]);

// @func getTokens
// @brief tokenizes user input on spaces
// @param input user-received string
// @param tokens array to place tokens into
// @return an array of tokens
void getTokens(char *input, char *tokens[]);

// @func findOutputRedirect
// @brief linearly searches for ">>" token
// @param tokens array of tokens to search
// @return index of first ">>" token, -1 if not found
int findOutputRedirect(char *tokens[]);

// @func findInputRedirect
// @brief linearly searches for "<<" token
// @param tokens array of tokens to search
// @return index of first "<<" token, -1 if not found
int findInputRedirect(char *tokens[]);

// @func findPipe
// @brief linearly searches for "|" token
// @param tokens array of tokens to search
// @return index of first "|" token, -1 if not found
int findPipe(char *tokens[]);

// @func findErr
// @brief linearly searches for "2>" token
// @param tokens array of tokens to search
// @return index of first "2>" token, -1 if not found
int findErr(char *tokens[]);

