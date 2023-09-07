#include "termtool.h"

#include <signal.h>  // for signal definitions
#include <stdio.h>
#include <sys/ioctl.h>  // ioctls for terminal query
#include <termios.h>    // for tcsetattr and others
#include <unistd.h>

static int g_cols_ = 0;
static int g_rows_ = 0;

namespace toolbox {

void move_cursor_up_rows(int row) noexcept { printf("\033[%dA", row); }
void clrscr() noexcept { printf("\u001b[1;1H\u001b[2J"); }
void hidecursor() noexcept { printf("\u001b[?25l"); }
void showcursor() noexcept { printf("\u001b[?25h"); }
int cols() noexcept { return g_cols_; }
int rows() noexcept { return g_rows_; }

/*  Function to disable echo as we type
 *  Read this for lots of good info: https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 */
static termios orig_tc;

void SetRawTerminal() noexcept {
  struct termios tc;
  tcgetattr(STDIN_FILENO, &orig_tc);
  tc = orig_tc;
  tc.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  tc.c_oflag &= ~(OPOST);
  tc.c_cflag |= (CS8);
  tc.c_lflag &= ~(ICANON | ECHO | IEXTEN);
  tc.c_cc[VMIN] = 0;
  tc.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &tc);
}

/*  Function to enable echo as we type
 *
 */
void RevertTerminalSettings() noexcept { tcsetattr(STDIN_FILENO, TCSANOW, &orig_tc); }

/*  Function to handle terminal resizing
 *
 */
static void handle_winch(int sig) {
  (void)sig;
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  g_cols_ = w.ws_col;
  g_rows_ = w.ws_row;
  clrscr();
}

void InitTerminalSize() noexcept {
  struct winsize w;

  signal(SIGWINCH, handle_winch);

  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  g_cols_ = w.ws_col;
  g_rows_ = w.ws_row;
}

}  // namespace toolbox
