int tty_send_xchar(struct tty_struct *tty, char ch)
{
	int	was_stopped = tty->stopped;

	if (tty->ops->send_xchar) {
		down_read(&tty->termios_rwsem);
		tty->ops->send_xchar(tty, ch);
		up_read(&tty->termios_rwsem);
		return 0;
	}

	if (tty_write_lock(tty, 0) < 0)
		return -ERESTARTSYS;

	down_read(&tty->termios_rwsem);
	if (was_stopped)
		start_tty(tty);
	tty->ops->write(tty, &ch, 1);
	if (was_stopped)
		stop_tty(tty);
	up_read(&tty->termios_rwsem);
	tty_write_unlock(tty);
	return 0;
}
