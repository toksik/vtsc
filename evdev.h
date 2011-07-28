#ifndef _VTSC_EVDEV_H
#define _VTSC_EVDEV_H

#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

/* The evdev interface only uses the minor numbers from 64 to 92. So there
   won't be more than 32 evdev device files. */
#define VTSC_MAXFDS 32

#define VTSC_EVKEY_MAX 800
#define VTSC_EVENT_LID 801
#define VTSC_EVCODE_MAX 801

struct vtsc_evfds
{
  int length;
  struct pollfd fds[VTSC_MAXFDS];
};

/**
 * Read an event (16 bytes) from 'fd' into 'ev'.
 */
void vtsc_readevent(int fd, struct input_event *ev);

/**
 * Wait for the next useful event and return the event code.
 */
int vtsc_getevent(struct vtsc_evfds *evfds, int *value);

/**
 * Add all event devices /dev/input/ to the fds->fds array.
 */
void vtsc_evdevsadd(struct vtsc_evfds *fds);

#endif
