// File: parse.h
// Author: Jarrad Cisco
// UT eid: jcc5225
// Description:
// Contains function prototypes for parse.c


// @func getArgs
// @brief extracts command arguments from an array of user-received tokens
// @param tokens array of tokens
// @param args array of arguments to fill
// @return void
void getArgs(char *tokens[], char *args[]);

// @func getTokens
// @brief tokenizes user input on spaces
// @param input user-received string
// @return an array of tokens
char **getTokens(char *input);

