#pragma once

namespace toolbox {

// Reset terminal colors
constexpr const char* RESET = "\u001b[0m";

// Erase line to the end
constexpr const char* ERASE_TO_END = "\033[K";

// Terminal decorations
constexpr const char* BOLD = "\u001b[1m";
constexpr const char* UNDERLINE = "\u001b[4m";
constexpr const char* REVERSED = "\u001b[7m";

// Foreground colors
constexpr const char* BLUE = "\033[0;34m";
constexpr const char* RED = "\033[0;31m";
constexpr const char* GREEN = "\033[0;32m";
constexpr const char* WHITE = "\033[1;37m";
constexpr const char* BLACK = "\u001b[30m";
constexpr const char* YELLOW = "\u001b[33m";
constexpr const char* MAGENTA = "\u001b[35m";
constexpr const char* CYAN = "\u001b[36m";

// Background colors
constexpr const char* BG_BLACK = "\u001b[40m";
constexpr const char* BG_RED = "\u001b[41m";
constexpr const char* BG_GREEN = "\u001b[42m";
constexpr const char* BG_YELLOW = "\u001b[43m";
constexpr const char* BG_BLUE = "\u001b[44m";
constexpr const char* BG_MAGENTA = "\u001b[45m";
constexpr const char* BG_CYAN = "\u001b[46m";
constexpr const char* BG_WHITE = "\u001b[47m";

void move_cursor_up_rows(int row) noexcept;

// This function clears the whole terminal screen
void clrscr() noexcept;

void hidecursor() noexcept;
void showcursor() noexcept;

// Raw terminal is useful to disable echo, capture keys without buffer
void SetRawTerminal() noexcept;

// Call this at exit to let the terminal be clean
void RevertTerminalSettings() noexcept;

// Only call this once per application
void InitTerminalSize() noexcept;
// Only call these functions after InitTerminalSize
int cols() noexcept;
int rows() noexcept;

}  // namespace toolbox
