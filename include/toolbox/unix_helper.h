#pragma once

// Execute a command; output is returned in 'output'.  Returns true if
// successful, false if not.
bool exec(const char* cmd, std::string &output);
