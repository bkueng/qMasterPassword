/*
 * Copyright (C) 2015 Beat Küng <beat-kueng@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/** linux-specific implementation of keypress functionality */

#include "keypress.h"
#include "exception.h"

#if defined(__linux__)

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XTest.h>
#include <cwchar>
#include <cstring>

typedef struct charcodemap {
	wchar_t key; /** the letter for this key, like 'a' */
	KeyCode code; /** the keycode that this key is on */
	KeySym symbol; /** the symbol representing this key */
	int index; /** the index in the keysym-per-keycode list that is this key */
	int modmask; /** the modifiers activated by this key */
	/** if this key need to be bound at runtime because it does not
	 * exist in the current keymap, this will be set to 1. */
	int needs_binding;
} charcodemap_t;

typedef struct keysym_charmap {
	const char *keysym;
	wchar_t key;
} keysym_charmap_t;

typedef struct xdo {
	/** The Display for Xlib */
	Display *xdpy;
	/** @internal Array of known keys/characters */
	charcodemap_t *charcodes = nullptr;
	/** @internal Lenth of charcodes array */
	int charcodes_len;
	/** @internal result from XGetModifierMapping */
	XModifierKeymap *modmap;
	/** @internal current keyboard mapping (via XGetKeyboardMapping) */
	KeySym *keymap;
	/** @internal highest keycode value */
	int keycode_high; /* highest and lowest keycodes */
	/** @internal number of keysyms per keycode */
	int keysyms_per_keycode;
	/** @internal lowest keycode value */
	int keycode_low; /* used by this X server */
} xdo_t;

typedef struct xdo_active_mods {
	charcodemap_t *keymods;
	int nkeymods;
	unsigned int input_state;
} xdo_active_mods_t;

int xdo_type(const xdo_t *xdo, const char *string, useconds_t delay);
static void _xdo_populate_charcode_map(xdo_t *xdo);
xdo_active_mods_t *xdo_get_active_modifiers(const xdo_t *xdo);
void xdo_free_active_modifiers(xdo_active_mods_t *active_mods);
int xdo_clear_active_modifiers(const xdo_t *xdo, xdo_active_mods_t *active_mods);
int xdo_set_active_modifiers(const xdo_t *xdo, const xdo_active_mods_t *active_mods);



Keypress::Keypress() {
	m_display = XOpenDisplay(NULL);
	if (!m_display) THROW_s(EFAILED_TO_LOAD, "XOpenDisplay failed");

	m_xdo = new xdo_t;
	m_xdo->xdpy = (Display*)m_display;
	_xdo_populate_charcode_map(m_xdo);
}

Keypress::~Keypress() {
	if (m_display) XCloseDisplay((Display*)m_display);
	if (m_xdo->charcodes) free(m_xdo->charcodes);
	if (m_xdo) delete (m_xdo);
	if (m_active_mods)
		xdo_free_active_modifiers((xdo_active_mods_t*)m_active_mods);
}

void Keypress::releaseModifiers() {
	if (m_active_mods)
		xdo_free_active_modifiers((xdo_active_mods_t*)m_active_mods);
	m_active_mods = xdo_get_active_modifiers(m_xdo);
	xdo_clear_active_modifiers(m_xdo, (xdo_active_mods_t*)m_active_mods);
}

void Keypress::restoreModifiers() {
	if (m_active_mods) {
		xdo_active_mods_t* active_mods = (xdo_active_mods_t*)m_active_mods;
		xdo_set_active_modifiers(m_xdo, active_mods);
		xdo_free_active_modifiers(active_mods);
		m_active_mods = nullptr;
	}
}

void Keypress::altTab() {
   unsigned int keycodeAlt, keycodeTab;
   Display* display = (Display*)m_display;

   keycodeAlt = XKeysymToKeycode(display, XK_Alt_L);
   keycodeTab = XKeysymToKeycode(display, XK_Tab);

   /* press alt */
   XTestFakeKeyEvent(display, keycodeAlt, True, 0);
   /* press tab */
   XTestFakeKeyEvent(display, keycodeTab, True, 100);

   /* release tab */
   XTestFakeKeyEvent(display, keycodeTab, False, 0);
   /* release alt */
   XTestFakeKeyEvent(display, keycodeAlt, False, 50);
   XFlush(display);
}

void Keypress::type(const char* str) {
	int key_udelay = 12000;
	xdo_type(m_xdo, str, key_udelay);
}

/**
 * the following code is taken from xdotool. Thanks Jordan!
 * Copyright (c) 2007, 2008, 2009: Jordan Sissel.
 * All rights reserved.
 *
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jordan Sissel nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY JORDAN SISSEL ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL JORDAN SISSEL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <clocale>

static const keysym_charmap_t keysym_charmap[] = { { "Return", '\n', }, {
		"ampersand", '&', }, { "apostrophe", '\'', }, { "asciicircum", '^', }, {
		"asciitilde", '~', }, { "asterisk", '*', }, { "at", '@', }, {
		"backslash", '\\', }, { "bar", '|', }, { "braceleft", '{', }, {
		"braceright", '}', }, { "bracketleft", '[', }, { "bracketright", ']', },
		{ "colon", ':', }, { "comma", ',', }, { "dollar", '$', },
		{ "equal", '=', }, { "exclam", '!', }, { "grave", '`', }, { "greater",
				'>', }, { "less", '<', }, { "minus", '-', },
		{ "numbersign", '#', }, { "parenleft", '(', }, { "parenright", ')', }, {
				"percent", '%', }, { "period", '.', }, { "plus", '+', }, {
				"question", '?', }, { "quotedbl", '"', }, { "semicolon", ';', },
		{ "slash", '/', }, { "space", ' ', }, { "tab", '\t', }, { "underscore",
				'_', }, { "Escape", '\x1b', }, { NULL, 0, }, };


/* human to Keysym string mapping */
static const char *symbol_map[] = {
  "alt", "Alt_L",
  "ctrl", "Control_L",
  "control", "Control_L",
  "meta", "Meta_L",
  "super", "Super_L",
  "shift", "Shift_L",
  NULL, NULL,
};

/* I can't find a constant in Xlib that is 0x2000 (or 1 << 13)
 * Maybe it's in Xkb? Either way, 0x2000 is the state shown by xev(1)
 * when we are shifted using the keyboard group shifting.  Example:
 * % setxkbmap -option * grp:switch,grp:shifts_toggle us,se
 * Then hit both shift-keys simultaneously to switch to 'se' key layout
 * and back again. */
#define ModeSwitchMask 0x2000

#define DEFAULT_DELAY 12


static KeyCode _xdo_keycode_from_char(const xdo_t *xdo, wchar_t key) {
	int i = 0;
	int len = xdo->charcodes_len;

	for (i = 0; i < len; i++) {
		if (xdo->charcodes[i].key == key) {
			return xdo->charcodes[i].code;
		}
	}

	return 0;
}

static KeySym _xdo_keysym_from_char(const xdo_t *xdo, wchar_t key) {
	int i = 0;
	int len = xdo->charcodes_len;

	//printf("Finding symbol for key '%c'\n", key);
	for (i = 0; i < len; i++) {
		//printf("  => %c vs %c (%d)\n",
		//key, xdo->charcodes[i].key, (xdo->charcodes[i].key == key));
		if (xdo->charcodes[i].key == key) {
			//printf("  => MATCH to symbol: %lu\n", xdo->charcodes[i].symbol);
			return xdo->charcodes[i].symbol;
		}
	}

	if (key >= 0x100)
		key += 0x01000000;
	if (XKeysymToString(key))
		return key;
	return NoSymbol;
}

static int _xdo_get_key_index(const xdo_t *xdo, wchar_t key) {
	int i = 0;
	int len = xdo->charcodes_len;

	for (i = 0; i < len; i++)
		if (xdo->charcodes[i].key == key)
			return xdo->charcodes[i].index;

	return -1;
}

int _xdo_cached_modifier_to_keycode(const xdo_t *xdo, int modmask) {
	int i = 0;

	for (i = 0; i < xdo->charcodes_len; i++)
		if (xdo->charcodes[i].modmask == modmask)
			return xdo->charcodes[i].code;

	return 0;
}

int _xdo_cached_keycode_to_modifier(const xdo_t *xdo, KeyCode keycode) {
	int i = 0;
	int len = xdo->charcodes_len;

	for (i = 0; i < len; i++)
		if (xdo->charcodes[i].code == keycode)
			return xdo->charcodes[i].modmask;

	return 0;
}

#include <unistd.h>

void _xdo_send_key(const xdo_t *xdo, charcodemap_t *key, int modstate,
		int is_press, useconds_t delay) {
	/* Properly ensure the modstate is set by finding a key
	 * that activates each bit in the modifier state */
	int mask = modstate | key->modmask;
	int masks[] = { ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask,
	Mod3Mask, Mod4Mask, Mod5Mask };
	unsigned int i = 0;
	if (mask != 0) {
		unsigned int masks_len = sizeof(masks) / sizeof(masks[0]);
		for (i = 0; i < masks_len; i++) { /* length of masks array above */
			if (mask & masks[i]) {
				KeyCode modkey;
				modkey = _xdo_cached_modifier_to_keycode(xdo, masks[i]);
				XTestFakeKeyEvent(xdo->xdpy, modkey, is_press, CurrentTime);
				XSync(xdo->xdpy, False);
			} /* if modestate includes this mask */
		} /* loop over possible masks */
	} /* if we have a mask set */

	XTestFakeKeyEvent(xdo->xdpy, key->code, is_press, CurrentTime);
	XSync(xdo->xdpy, False);

	XFlush(xdo->xdpy);
	if (delay > 0)
		usleep(delay);
}

int xdo_keysequence_list_do(const xdo_t *xdo, charcodemap_t *keys, int nkeys,
		int pressed, int *modifier, useconds_t delay) {
	int i = 0;
	int modstate = 0;
	int keymapchanged = 0;

	/* Find an unused keycode in case we need to bind unmapped keysyms */
	KeySym *keysyms = NULL;
	int keysyms_per_keycode = 0;
	int scratch_keycode = 0; /* Scratch space for temporary keycode bindings */
	/* Find a keycode that is unused for scratchspace */
	for (i = xdo->keycode_low; i <= xdo->keycode_high; i++) {
		int j = 0;
		int key_is_empty = 1;
		for (j = 0; j < keysyms_per_keycode; j++) {
			int symindex = (i - xdo->keycode_low) * keysyms_per_keycode + j;
			if (keysyms[symindex] != 0) {
				key_is_empty = 0;
			} else {
				break;
			}
		}
		if (key_is_empty) {
			scratch_keycode = i;
			break;
		}
	}
	XFree(keysyms);

	/* Allow passing NULL for modifier in case we don't care about knowing
	 * the modifier map state after we finish */
	if (modifier == NULL)
		modifier = &modstate;

	for (i = 0; i < nkeys; i++) {
		if (keys[i].needs_binding == 1) {
			KeySym keysym_list[] = { keys[i].symbol };
			XChangeKeyboardMapping(xdo->xdpy, scratch_keycode, 1, keysym_list,
					1);
			XSync(xdo->xdpy, False);
			/* override the code in our current key to use the scratch_keycode */
			keys[i].code = scratch_keycode;
			keymapchanged = 1;
		}

		_xdo_send_key(xdo, &(keys[i]), *modifier, pressed, delay);

		if (keys[i].needs_binding == 1) {
			/* If we needed to make a new keymapping for this keystroke, we
			 * should sync with the server now, after the keypress, so that
			 * the next mapping or removal doesn't conflict. */
			XSync(xdo->xdpy, False);
		}

		if (pressed) {
			*modifier |= _xdo_cached_keycode_to_modifier(xdo, keys[i].code);
		} else {
			*modifier &= ~(_xdo_cached_keycode_to_modifier(xdo, keys[i].code));
		}
	}

	if (keymapchanged) {
		KeySym keysym_list[] = { 0 };
		XChangeKeyboardMapping(xdo->xdpy, scratch_keycode, 1, keysym_list, 1);
	}

	XFlush(xdo->xdpy);
	return 0;
}

int xdo_active_keys_to_keycode_list(const xdo_t *xdo, charcodemap_t **keys,
		int *nkeys) {
	/* For each keyboard device, if an active key is a modifier,
	 * then add the keycode to the keycode list */

	char keymap[32]; /* keycode map: 256 bits */
	int keys_size = 10;
	int keycode = 0;
	*nkeys = 0;
	*keys = (charcodemap_t*) malloc(keys_size * sizeof(charcodemap_t));

	XQueryKeymap(xdo->xdpy, keymap);

	for (keycode = xdo->keycode_low; keycode <= xdo->keycode_high; keycode++) {
		if ((keymap[(keycode / 8)] & (1 << (keycode % 8)))
				&& _xdo_cached_keycode_to_modifier(xdo, keycode)) {
			/* This keycode is active and is a modifier, record it. */

			/* Zero the charcodemap_t entry before using it.
			 * Fixes a bug reported by Hong-Leong Ong - where
			 * 'xdotool key --clearmodifiers ...' sometimes failed trying
			 * to clear modifiers that didn't exist since charcodemap_t's modmask was
			 * uninitialized */
			memset(*keys + *nkeys, 0, sizeof(charcodemap_t));

			(*keys)[*nkeys].code = keycode;
			(*nkeys)++;

			if (*nkeys == keys_size) {
				keys_size *= 2;
				*keys = (charcodemap_t*) realloc(*keys,
						keys_size * sizeof(charcodemap_t));
			}
		}
	}

	return 0;
}

unsigned int xdo_get_input_state(const xdo_t *xdo) {
	Window root, dummy;
	int root_x, root_y, win_x, win_y;
	unsigned int mask;
	root = DefaultRootWindow(xdo->xdpy);

	XQueryPointer(xdo->xdpy, root, &dummy, &dummy, &root_x, &root_y, &win_x,
			&win_y, &mask);
	return mask;
}

xdo_active_mods_t *xdo_get_active_modifiers(const xdo_t *xdo) {
	xdo_active_mods_t *active_mods = NULL;

	active_mods = (xdo_active_mods_t*) calloc(sizeof(xdo_active_mods_t), 1);
	xdo_active_keys_to_keycode_list(xdo, &(active_mods->keymods),
			&(active_mods->nkeymods));
	active_mods->input_state = xdo_get_input_state(xdo);
	return active_mods;
}
void xdo_free_active_modifiers(xdo_active_mods_t *active_mods) {
	free(active_mods->keymods);
	free(active_mods);
}

wchar_t _keysym_to_char(const char *keysym) {
	int i;

	if (keysym == NULL)
		return -1;

	/* keysym_charmap comes from xdo_util.h */
	for (i = 0; keysym_charmap[i].keysym; i++) {
		if (!strcmp(keysym_charmap[i].keysym, keysym)) {
			//printf("keysym (%s) %d => '%c'\n", keysym,
			//keysym_charmap[i].keysym, keysym_charmap[i].key);
			return keysym_charmap[i].key;
		}
	}

	if (strlen(keysym) == 1)
		return keysym[0];

	return '\0';
}

int _xdo_keysequence_to_keycode_list(const xdo_t *xdo, const char *keyseq,
		charcodemap_t **keys, int *nkeys) {
	char *tokctx = NULL;
	const char *tok = NULL;
	char *keyseq_copy = NULL, *strptr = NULL;
	int i = 0;
	int shift_keycode = 0;
	int input_state = 0;

	/* Array of keys to press, in order given by keyseq */
	int keys_size = 10;

	if (strcspn(keyseq, " \t\n.-[]{}\\|") != strlen(keyseq)) {
		fprintf(stderr, "Error: Invalid key sequence '%s'\n", keyseq);
		return False;
	}

	shift_keycode = XKeysymToKeycode(xdo->xdpy, XStringToKeysym("Shift_L"));
	input_state = xdo_get_input_state(xdo);

	*nkeys = 0;
	*keys = (charcodemap_t*) calloc(keys_size, sizeof(charcodemap_t));
	keyseq_copy = strptr = strdup(keyseq);
	while ((tok = strtok_r(strptr, "+", &tokctx)) != NULL) {
		KeySym sym;
		KeyCode key;

		if (strptr != NULL)
			strptr = NULL;

		/* Check if 'tok' (string keysym) is an alias to another key */
		/* symbol_map comes from xdo.util */
		for (i = 0; symbol_map[i] != NULL; i += 2)
			if (!strcasecmp(tok, symbol_map[i]))
				tok = symbol_map[i + 1];

		sym = XStringToKeysym(tok);
		if (sym == NoSymbol) {
			/* Accept a number as a explicit keycode */
			if (isdigit(tok[0])) {
				key = (unsigned int) atoi(tok);
			} else {
				fprintf(stderr,
						"(symbol) No such key name '%s'. Ignoring it.\n", tok);
				continue;
			}
		} else {
			key = XKeysymToKeycode(xdo->xdpy, sym);

			/* Hack for international/modeshift things.
			 * If we can't type this keysym with just a keycode or shift+keycode,
			 * let's pretend we didn't find the keycode and request
			 * a keybinding.
			 *
			 * Should fix these bugs:
			 * http://code.google.com/p/semicomplete/issues/detail?id=21
			 * http://code.google.com/p/semicomplete/issues/detail?id=13
			 *
			 * This hack seems better (less code) than walking the keymap to find
			 * which modifiers are required to type with this keycode to invoke
			 * this keysym.
			 */

			int offset = 0;
			if (input_state & ModeSwitchMask) { /* keymap shifted via xkb */
				offset = 2;
			}

			if (XkbKeycodeToKeysym(xdo->xdpy, key, 0, 0 + offset) != sym
					&& XkbKeycodeToKeysym(xdo->xdpy, key, 0, 1 + offset) != sym) {
				key = 0;
			}
		}

		if (key == 0) {
			//fprintf(stderr, "No such key '%s'. Ignoring it.\n", tok);
			(*keys)[*nkeys].symbol = sym;
			(*keys)[*nkeys].needs_binding = 1;
			(*keys)[*nkeys].code = 0;
		} else {
			/* Inject a shift key if we need to press shift to reach this keysym */
			//if (xdo->keymap[key * xdo->keysyms_per_keycode] == sym
			//|| sym == NoSymbol) {
			if ((XkbKeycodeToKeysym(xdo->xdpy, key, 0, 0) == sym) || sym == NoSymbol) {
				/* sym is NoSymbol if we give a keycode to type */
				(*keys)[*nkeys].index = 0;
			} else {
				/* Inject a 'shift' key item if we should be using shift */
				(*keys)[*nkeys].symbol = NoSymbol;
				(*keys)[*nkeys].code = shift_keycode;
				(*keys)[*nkeys].needs_binding = 0;
				(*keys)[*nkeys].index = 0;
				(*nkeys)++;

				if (*nkeys == keys_size) {
					keys_size *= 2;
					*keys = (charcodemap_t*) realloc(*keys,
							keys_size * sizeof(charcodemap_t));
				}
			}

			/* Record the original keycode */
			(*keys)[*nkeys].symbol = NoSymbol;
			(*keys)[*nkeys].needs_binding = 0;
			(*keys)[*nkeys].code = key;
			(*keys)[*nkeys].modmask = 0;
			(*keys)[*nkeys].key = _keysym_to_char(tok);
			//printf("Ready for %s\n", tok);
		}

		(*nkeys)++;
		if (*nkeys == keys_size) {
			keys_size *= 2;
			*keys = (charcodemap_t*) realloc(*keys,
					keys_size * sizeof(charcodemap_t));
		}
	}

	free(keyseq_copy);
	return True;
}

int _xdo_keysequence_do(const xdo_t *xdo, const char *keyseq, int pressed,
		int *modifier, useconds_t delay) {
	int ret = 0;
	charcodemap_t *keys = NULL;
	int nkeys = 0;

	if (_xdo_keysequence_to_keycode_list(xdo, keyseq, &keys, &nkeys) == False) {
		fprintf(stderr, "Failure converting key sequence '%s' to keycodes\n",
				keyseq);
		return 1;
	}

	ret = xdo_keysequence_list_do(xdo, keys, nkeys, pressed, modifier, delay);
	if (keys != NULL) {
		free(keys);
	}

	return ret;
}

int xdo_keysequence_down(const xdo_t *xdo, const char *keyseq,
		useconds_t delay) {
	return _xdo_keysequence_do(xdo, keyseq, True, NULL, delay);
}

int xdo_keysequence_up(const xdo_t *xdo, const char *keyseq, useconds_t delay) {
	return _xdo_keysequence_do(xdo, keyseq, False, NULL, delay);
}

int xdo_clear_active_modifiers(const xdo_t *xdo,
		xdo_active_mods_t *active_mods) {
	int ret = 0;
	xdo_keysequence_list_do(xdo, active_mods->keymods, active_mods->nkeymods,
			False, NULL, DEFAULT_DELAY);

	/*
	 //FIXME: we ignore the mouse here. is that ok in all cases?
	 if (active_mods->input_state & Button1MotionMask)
	 ret = xdo_mouseup(xdo, 1);
	 if (!ret && active_mods->input_state & Button2MotionMask)
	 ret = xdo_mouseup(xdo, 2);
	 if (!ret && active_mods->input_state & Button3MotionMask)
	 ret = xdo_mouseup(xdo, 3);
	 if (!ret && active_mods->input_state & Button4MotionMask)
	 ret = xdo_mouseup(xdo, 4);
	 if (!ret && active_mods->input_state & Button5MotionMask)
	 ret = xdo_mouseup(xdo, 5);
	 */
	if (!ret && active_mods->input_state & LockMask) {
		/* explicitly use down+up here since xdo_keysequence alone will track the modifiers
		 * incurred by a key (like shift, or caps) and send them on the 'up' sequence.
		 * That seems to break things with Caps_Lock only, so let's be explicit here. */
		ret = xdo_keysequence_down(xdo, "Caps_Lock", DEFAULT_DELAY);
		ret += xdo_keysequence_up(xdo, "Caps_Lock", DEFAULT_DELAY);
	}

	XSync(xdo->xdpy, False);
	return ret;
}

int xdo_set_active_modifiers(const xdo_t *xdo,
		const xdo_active_mods_t *active_mods) {
	int ret = 0;
	xdo_keysequence_list_do(xdo, active_mods->keymods, active_mods->nkeymods,
			True, NULL, DEFAULT_DELAY);
	/*
	 //FIXME: we ignore the mouse here. is that ok in all cases?
	 if (active_mods->input_state & Button1MotionMask)
	 ret = xdo_mousedown(xdo, window, 1);
	 if (!ret && active_mods->input_state & Button2MotionMask)
	 ret = xdo_mousedown(xdo, window, 2);
	 if (!ret && active_mods->input_state & Button3MotionMask)
	 ret = xdo_mousedown(xdo, window, 3);
	 if (!ret && active_mods->input_state & Button4MotionMask)
	 ret = xdo_mousedown(xdo, window, 4);
	 if (!ret && active_mods->input_state & Button5MotionMask)
	 ret = xdo_mousedown(xdo, window, 5);
	 */
	if (!ret && active_mods->input_state & LockMask) {
		/* explicitly use down+up here since xdo_keysequence alone will track the modifiers
		 * incurred by a key (like shift, or caps) and send them on the 'up' sequence.
		 * That seems to break things with Caps_Lock only, so let's be explicit here. */
		ret = xdo_keysequence_down(xdo, "Caps_Lock", DEFAULT_DELAY);
		ret += xdo_keysequence_up(xdo, "Caps_Lock", DEFAULT_DELAY);
	}

	XSync(xdo->xdpy, False);
	return ret;
}


int xdo_type(const xdo_t *xdo, const char *string, useconds_t delay) {

	/* Since we're doing down/up, the delay should be based on the number
	 * of keys pressed (including shift). Since up/down is two calls,
	 * divide by two. */
	delay /= 2;

	xdo_active_mods_t *current_mods = xdo_get_active_modifiers(xdo);
	charcodemap_t key;
	setlocale(LC_CTYPE, "");
	mbstate_t ps;
	memset(&ps, 0, sizeof(mbstate_t));
	ssize_t len;
	while ((len = mbsrtowcs(&key.key, &string, 1, &ps))) {
		if (len == -1) {
			fprintf(stderr, "Invalid multi-byte sequence encountered\n");
			return -1;
		}
		key.code = _xdo_keycode_from_char(xdo, key.key);
		key.symbol = _xdo_keysym_from_char(xdo, key.key);
		key.modmask = 0;
		key.needs_binding = 0;
		if (key.code == 0 && key.symbol == NoSymbol) {
			/* Try the charmap */
			int kci = 0;
			//printf("Can't find key %c, checking charmap\n", key.key);
			for (kci = 0; keysym_charmap[kci].keysym; kci++) {
				if (key.key == keysym_charmap[kci].key) {
					key.symbol = XStringToKeysym(keysym_charmap[kci].keysym);
				}
			}

			if (key.symbol == NoSymbol) {
				fprintf(stderr, "I don't what key produces '%lc', skipping.\n",
						key.key);
				continue;
			}
		} else {
			//printf("Found key for %c\n", key.key);
			//printf("code: %d\n", key.code);
			//printf("sym: %s\n", XKeysymToString(key.symbol));
		}

		if (key.code > 0) {
			key.index = _xdo_get_key_index(xdo, key.key);
		} else {
			key.needs_binding = 1;
		}

		/* I don't know how to type keys beyond key group 1 or 2 (index 4 and
		 * beyond). Index 4 and 5 are suppsedly means numlock is set. However,
		 * simply sending the Num_Lock key doesn't seem to work. We can work
		 * around this by binding a new key to the key and using that. */
		if (key.index >= 4) {
			key.needs_binding = 1;
		}

		if (key.needs_binding == 0) {
			if (key.index & 1) { /* odd numbered index are shifted */
				key.modmask |= ShiftMask;
			}
			/* Keys with index 2 and 3 are accessed with Mode_switch key, which is
			 * defaults to Mod5Mask */
			if ((current_mods->input_state & ModeSwitchMask) == 0) {
				if (key.index == 2 || key.index == 3) {
					key.modmask |= Mod5Mask; /* Set AltG/Mode_Shift */
				}
			}
		}

		xdo_keysequence_list_do(xdo, &key, 1, True, NULL, delay / 2);
		key.needs_binding = 0;
		xdo_keysequence_list_do(xdo, &key, 1, False, NULL, delay / 2);

	} /* walk string generating a keysequence */

	xdo_free_active_modifiers(current_mods);
	return 0;
}

int _xdo_query_keycode_to_modifier(const xdo_t *xdo, KeyCode keycode) {
	int i = 0, j = 0;
	int max = xdo->modmap->max_keypermod;

	for (i = 0; i < 8; i++) { /* 8 modifier types, per XGetModifierMapping(3X) */
		for (j = 0; j < max && xdo->modmap->modifiermap[(i * max) + j]; j++) {
			if (keycode == xdo->modmap->modifiermap[(i * max) + j]) {
				switch (i) {
				case ShiftMapIndex:
					return ShiftMask;
					break;
				case LockMapIndex:
					return LockMask;
					break;
				case ControlMapIndex:
					return ControlMask;
					break;
				case Mod1MapIndex:
					return Mod1Mask;
					break;
				case Mod2MapIndex:
					return Mod2Mask;
					break;
				case Mod3MapIndex:
					return Mod3Mask;
					break;
				case Mod4MapIndex:
					return Mod4Mask;
					break;
				case Mod5MapIndex:
					return Mod5Mask;
					break;
				}
			} /* end if */
		} /* end loop j */
	} /* end loop i */

	/* No modifier found for this keycode, return no mask */
	return 0;
}

static void _xdo_populate_charcode_map(xdo_t *xdo) {
	/* assert xdo->display is valid */
	int keycodes_length = 0;
	int i, j;

	XDisplayKeycodes(xdo->xdpy, &(xdo->keycode_low), &(xdo->keycode_high));
	xdo->modmap = XGetModifierMapping(xdo->xdpy);
	xdo->keymap = XGetKeyboardMapping(xdo->xdpy, xdo->keycode_low,
			xdo->keycode_high - xdo->keycode_low + 1,
			&xdo->keysyms_per_keycode);

	/* Add 2 to the size because the range [low, high] is inclusive */
	/* Add 2 more for tab (\t) and newline (\n) */
	keycodes_length = (((xdo->keycode_high - xdo->keycode_low) + 1)
			* xdo->keysyms_per_keycode) + (2 + 2);
	xdo->charcodes_len = keycodes_length;
	xdo->charcodes = (charcodemap_t*) calloc(keycodes_length,
			sizeof(charcodemap_t));

	int idx = 0;
	for (i = xdo->keycode_low; i <= xdo->keycode_high; i++) {
		char *keybuf = 0;

		/* Index '0' in KeycodeToKeysym == no shift key
		 * Index '1' in ... == shift key held
		 * hence this little loop. */
		for (j = 0; j < xdo->keysyms_per_keycode; j++) {
			//KeySym keysym = XkbKeycodeToKeysym(xdo->xdpy, i, 0, j);
			int keymap_index = ((i - xdo->keycode_low)
					* xdo->keysyms_per_keycode) + j;
			KeySym keysym = xdo->keymap[keymap_index];
			if (keysym != NoSymbol) {
				keybuf = XKeysymToString(keysym);
			} else {
				keybuf = NULL;
			}

			xdo->charcodes[idx].key = _keysym_to_char(keybuf);
			xdo->charcodes[idx].code = i;
			xdo->charcodes[idx].index = j;
			xdo->charcodes[idx].modmask = _xdo_query_keycode_to_modifier(xdo,
					i);
			xdo->charcodes[idx].symbol = keysym;

			//printf("_xdo_populate_charcode_map(%d/%d). %d[%d] is %lu => %s aka '%c'\n",
			//keymap_index, keycodes_length, i, j, keysym, keybuf, xdo->charcodes[idx].key);
			idx++;
		}
	}

	/* Add special handling so we can translate ASCII newline and tab
	 * to keycodes */
	//j = (xdo->keycode_high - xdo->keycode_low) * xdo->modmap->max_keypermod;
	xdo->charcodes[idx].key = '\n';
	xdo->charcodes[idx].code = XKeysymToKeycode(xdo->xdpy, XK_Return);
	xdo->charcodes[idx].index = 0;
	xdo->charcodes[idx].modmask = 0;

	idx++;
	xdo->charcodes[idx].key = '\t';
	xdo->charcodes[idx].code = XKeysymToKeycode(xdo->xdpy, XK_Tab);
	xdo->charcodes[idx].index = 0;
	xdo->charcodes[idx].modmask = 0;
}

#endif /* defined(__linux__) */
