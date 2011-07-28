#include "shortcuts.h"

/**
 * Handle a keypress event and run the appropriate shortcut actions.
 *
 * The 'key' argument should be the (translated) event code like it is returned
 * by vtsc_getevent().
 */
void vtsc_keypress(struct vtsc_keystate *state, int key)
{
  struct vtsc_shortcut *sc;
  int sc_c, key_c, has_key;
  if (~state->keys[key] & VTSC_KEYUSED)
    return;
  state->keys[key] = state->keys[key] | VTSC_KEYPRESSED;
  sc = state->shortcuts;
  for (sc_c = 0; sc_c < state->length; ++ sc_c)
    {
      has_key = 0;
      for (key_c = 0; sc->keys[key_c] != 0; ++ key_c)
	{
	  if (sc->keys[key_c] == key)
	    has_key = 1;
	  if (~state->keys[sc->keys[key_c]] & VTSC_KEYPRESSED)
	    {
	      has_key = 0;
	      break;
	    }
	}
      if (has_key == 1)
	vtsc_doaction(sc);
      sc = sc->next;
    }
}

/**
 * Handle a keyrelease event
 */
void vtsc_keyrelease(struct vtsc_keystate *state, int key)
{
  if (~state->keys[key] & VTSC_KEYUSED)
    return;
  state->keys[key] = state->keys[key] ^ VTSC_KEYPRESSED;
}

/**
 * Set the flags of all keys in state->keys to 0.
 *
 * Do this before using the keystate struct with any other function.
 */
void vtsc_clearkeystate(struct vtsc_keystate *state)
{
  int key_c;
  for (key_c = 0; key_c < VTSC_EVCODE_MAX; ++ key_c)
    state->keys[key_c] = 0;
}

/**
 * Do the action specified in sc->type.
 */
void vtsc_doaction(struct vtsc_shortcut *sc)
{
  if (sc->type == VTSC_SCNONE)
    return;
  else if (sc->type == VTSC_SCDEBUG)
    {
      if (sc->action == NULL)
	printf("Debug shortcut: unknown\n");
      else
	printf("Debug shortcut: %s\n", sc->action);
    }
  else if (sc->type == VTSC_SCCOMMAND)
    {
      if (sc->action == NULL)
	fprintf(stderr, "Error: No command specified\n");
      else
	if (fork() == 0)
	  execl("/bin/sh", "/bin/sh", "-c", sc->action, NULL);
    }
}
