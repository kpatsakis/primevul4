void __start_tty(struct tty_struct *tty)
{
	if (!tty->stopped || tty->flow_stopped)
		return;
	tty->stopped = 0;
	if (tty->ops->start)
		tty->ops->start(tty);
	tty_wakeup(tty);
}
