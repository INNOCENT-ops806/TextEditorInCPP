#include <cerrno>
#include <cctype>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
using namespace std;

struct termios orig_termios;

void die(const char *s){
  cerr <<  "Error: " << s << endl;
  exit(1);
}


void disableRawMode() {
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  if(tcgetattr(STDIN_FILENO, &orig_termios) == -1 ) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int main() {
  enableRawMode();

  while (1) {
    char c = '\0';
    if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
    if (iscntrl(c))
      cout << static_cast<int>(c) << "\r\n";
    else
      cout << static_cast<int>(c) << " ('" << c << "')\r\n";
    if (c == 'q') break;
  }

  return 0;
}

