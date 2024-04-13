void __stop_tty(struct tty_struct *tty)
{
	if (tty->stopped)
		return;
	tty->stopped = 1;
	if (tty->ops->stop)
		tty->ops->stop(tty);
}
