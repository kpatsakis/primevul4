int tty_init_termios(struct tty_struct *tty)
{
	struct ktermios *tp;
	int idx = tty->index;

	tp = tty->driver->termios[idx];
	if (tp == NULL) {
		tp = kzalloc(sizeof(struct ktermios[2]), GFP_KERNEL);
		if (tp == NULL)
			return -ENOMEM;
		memcpy(tp, &tty->driver->init_termios,
						sizeof(struct ktermios));
		tty->driver->termios[idx] = tp;
	}
	tty->termios = tp;
	tty->termios_locked = tp + 1;

	/* Compatibility until drivers always set this */
	tty->termios->c_ispeed = tty_termios_input_baud_rate(tty->termios);
	tty->termios->c_ospeed = tty_termios_baud_rate(tty->termios);
	return 0;
}
