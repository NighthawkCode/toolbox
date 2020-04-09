#pragma once

// Execute a command; output is returned in 'output'.  Returns true if
// successful, false if not.
bool exec(const char* cmd, std::string &output);

/// get a character from stdin byte by byte
/// This is useful if we don't want to wait 
/// for the user to hit enter to process stdin
char getchar_non_canonical();
