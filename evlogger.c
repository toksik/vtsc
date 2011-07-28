#include <termios.h>
#include <signal.h>
#include "evdev.h"

struct termios term_old;

void vtsc_sigint_handler(int signum)
{
  tcsetattr(0, TCSANOW, &term_old);
  exit(0);
}

int main(int argc, char **argv)
{
  struct termios term_new;
  struct vtsc_evfds fds;
  int code, value;
  fds.length = 0;
  vtsc_evdevsadd(&fds);
  if (fds.length == 0)
    exit(-1); // fixme: display error message
  /* Turn echoing off */
  tcgetattr(0, &term_old);
  tcgetattr(0, &term_new);
  term_new.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &term_new);
  signal(SIGINT, vtsc_sigint_handler);
  while (1)
    {
      code = vtsc_getevent(&fds, &value);
      if (value == 0)
	printf("Key %i released\n", code);
      else if (value == 1)
	printf("Key %i pressed\n", code);
      //else if (value == 2)
      //  printf("Key %i repeated\n", code);
    }
}
