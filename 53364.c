void tty_free_termios(struct tty_struct *tty)
{
	struct ktermios *tp;
	int idx = tty->index;

	/* If the port is going to reset then it has no termios to save */
	if (tty->driver->flags & TTY_DRIVER_RESET_TERMIOS)
		return;

	/* Stash the termios data */
	tp = tty->driver->termios[idx];
	if (tp == NULL) {
		tp = kmalloc(sizeof(struct ktermios), GFP_KERNEL);
		if (tp == NULL)
			return;
		tty->driver->termios[idx] = tp;
	}
	*tp = tty->termios;
}
