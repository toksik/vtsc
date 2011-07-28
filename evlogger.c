#include <termios.h>
#include <signal.h>
#include <string.h>
#include "evdev.h"

struct termios term_old;

void vtsc_sigint_handler(int signum)
{
  tcsetattr(0, TCSANOW, &term_old);
  exit(0);
}

void vtsc_usage(char *path)
{
  fprintf(stderr, "Usage: %s [-aer] [-d device path]\n\
Displays incoming events from one or more input event devices\n\n\
 -a               Don't use the standard device files (/dev/input/event{0-32})\n\
 -d <device path> Use the given device file to get the input events.\n\
 -e               Don't turn echoing of.\n\
 -h               Display this message.\n\
 -r               Show repeat events\n\n\
It is possible to use multiple device files and even to use the standard\n\
ones together with custrom files\n", path);
  exit(0);
}

int main(int argc, char **argv)
{
  struct termios term_new;
  struct vtsc_evfds fds;
  int code, value;
  int optc;
  char repeat_opt = 0, autoadd_opt = 1, echo_opt = 0;
  fds.length = 0;
  while ((optc = getopt(argc, argv, "ad:ehr")) != -1)
    {
      switch (optc)
	{
	  /* Don't add standard event devices (/dev/input/event{0-32}) */
	case 'a':
	  autoadd_opt = 0;
	  break;
	  /* Add custrom event device */
	case 'd':
	  fds.fds[fds.length].fd = open(optarg, O_RDONLY);
	  if (fds.fds[fds.length].fd == -1)
	    {
	      char errstr[25+strlen(optarg)];
	      snprintf(errstr, 24+strlen(optarg), "Opening event device (%s)",
		       optarg);
	      perror(errstr);
	      break;
	    }
	  fds.fds[fds.length].events = POLLIN;
	  fds.fds[fds.length].revents = 0;
	  ++ fds.length;
	  break;
	  /* Don't turn echoing off */
	case 'e':
	  echo_opt = 1;
	  break;
	  /* Show key repeat events */
	case 'r':
	  repeat_opt = 1;
	  break;
	default:
	  vtsc_usage(argv[0]);
	}
    }
  if (autoadd_opt == 1)
    vtsc_evdevsadd(&fds);
  if (fds.length == 0)
    exit(-1); // fixme: display error message
  /* Turn echoing off */
  tcgetattr(0, &term_old);
  tcgetattr(0, &term_new);
  term_new.c_lflag &= ~ECHO;
  if (echo_opt == 0)
    tcsetattr(0, TCSANOW, &term_new);
  signal(SIGINT, vtsc_sigint_handler);
  while (1)
    {
      code = vtsc_getevent(&fds, &value);
      if (value == 0)
	printf("Key %i released\n", code);
      else if (value == 1)
	printf("Key %i pressed\n", code);
      else if ((value == 2) & (repeat_opt == 1))
        printf("Key %i repeated\n", code);
    }
}
