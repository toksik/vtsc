#ifndef _VTSC_SHORTCUTS_H
#define _VTSC_SHORTCUTS_H

#include <stdio.h>
#include <stdlib.h>
#include "evdev.h"

/* Key states */
#define VTSC_KEYPRESSED 1
#define VTSC_KEYUSED 2

/* Shortcut types */
#define VTSC_SCNONE 0
#define VTSC_SCDEBUG 1
#define VTSC_SCCOMMAND 2

struct vtsc_shortcut
{
  int *keys;
  char type;
  char *action;
  struct vtsc_shortcut *next;
};

struct vtsc_keystate
{
  int length;
  struct vtsc_shortcut *shortcuts;
  char keys[801];
};

/**
 * Handle a keypress event and run the appropriate shortcut actions.
 */
void vtsc_keypress(struct vtsc_keystate *state, int key);

/**
 * Handle a keyrelease event
 */
void vtsc_keyrelease(struct vtsc_keystate *state, int key);

/**
 * Set the flags of all keys in state->keys to 0.
 */
void vtsc_clearkeystate(struct vtsc_keystate *state);

/**
 * Do the action specified in sc->type.
 */
void vtsc_doaction(struct vtsc_shortcut *sc);

#endif
