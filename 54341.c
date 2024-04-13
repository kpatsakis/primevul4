void tty_free_termios(struct tty_struct *tty)
{
	struct ktermios *tp;
	int idx = tty->index;
	/* Kill this flag and push into drivers for locking etc */
	if (tty->driver->flags & TTY_DRIVER_RESET_TERMIOS) {
		/* FIXME: Locking on ->termios array */
		tp = tty->termios;
		tty->driver->termios[idx] = NULL;
		kfree(tp);
	}
}
