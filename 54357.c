void tty_shutdown(struct tty_struct *tty)
{
	tty_driver_remove_tty(tty->driver, tty);
	tty_free_termios(tty);
}
