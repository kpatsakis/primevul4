int tty_init_termios(struct tty_struct *tty)
{
	struct ktermios *tp;
	int idx = tty->index;

	if (tty->driver->flags & TTY_DRIVER_RESET_TERMIOS)
		tty->termios = tty->driver->init_termios;
	else {
		/* Check for lazy saved data */
		tp = tty->driver->termios[idx];
		if (tp != NULL)
			tty->termios = *tp;
		else
			tty->termios = tty->driver->init_termios;
	}
	/* Compatibility until drivers always set this */
	tty->termios.c_ispeed = tty_termios_input_baud_rate(&tty->termios);
	tty->termios.c_ospeed = tty_termios_baud_rate(&tty->termios);
	return 0;
}
