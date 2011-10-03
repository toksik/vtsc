#include "evdev.h"

/**
 * Read an event (usually 16 bytes) from 'fd' into 'ev'.
 */
void vtsc_readevent(int fd, struct input_event *ev)
{
  size_t len, chunk_len;
  len = 0;
  while (len < sizeof(struct input_event))
    {
      chunk_len = read(fd, ev, sizeof(struct input_event)-len);
      if (chunk_len == 0)
	exit(-1); // fixme: print useful error message
      else if (chunk_len == -1)
	{
	  perror("Reading event");
	  exit(-1);
	}
      len += chunk_len;
    }
}

/**
 * Wait for the next useful event and return the event code.
 *
 * It returns the translated event code and write the event value to 'value'
 * if it is not NULL.
 *
 * Event codes:
 * 0 - 800 Key Events
 * 801     Lid event
 */
int vtsc_getevent(struct vtsc_evfds *evfds, int *value)
{
  int fd_count, len, chunk_len;
  struct input_event ev;
  ev.type = -1;
  while (1)
    {
      if (poll(evfds->fds, evfds->length, -1) == -1)
	{
	  perror("Polling event devices");
	  exit(-1);
	}
      for (fd_count = 0; fd_count < evfds->length; ++ fd_count)
	{
	  if (~evfds->fds[fd_count].revents & POLLIN)
	    continue;
	  vtsc_readevent(evfds->fds[fd_count].fd, &ev);
	  /* Key events */
	  if (ev.type == EV_KEY)
	    {
	      if (value != NULL)
		*value = ev.value;
	      if (ev.code > VTSC_EVKEY_MAX)
		continue;
	      return ev.code;
	    }
	  /* Lid events */
	  else if ((ev.type == EV_SW) & (ev.code == SW_LID))
	    {
	      if (value != NULL)
		*value = ev.value;
	      return VTSC_EVENT_LID;
	    }
	}
    }
}

/**
 * Add all event devices /dev/input/ to the fds->fds array.
 *
 * It tries all event devices starting with /dev/input/event0 until stat fails.
 * Errors while opening the device files will be reported and just ignored.
 */
void vtsc_evdevsadd(struct vtsc_evfds *fds)
{
  int dev_counter, fd;
  /* "/dev/input/event" (16) + two digit number + \0 */ 
  char path[19];
  struct stat devstat;
  for (dev_counter = 0; dev_counter < VTSC_MAXFDS; ++ dev_counter)
    {
      if (fds->length >= VTSC_MAXFDS)
	break;
      snprintf(path, 18, "/dev/input/event%i", dev_counter);
      if (stat(path, &devstat) == -1)
	break;
      if (~devstat.st_mode & S_IFCHR)
	break;
      fd = open(path, O_RDONLY);
      if (fd == -1)
	{
	  char errstr[42];
	  snprintf(errstr, 41, "Opening event device (%s)", path);
	  continue;
	}
      fds->fds[fds->length].fd = fd;
      fds->fds[fds->length].events = POLLIN;
      fds->fds[fds->length].revents = 0;
      ++ fds->length;
    }
}
