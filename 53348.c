int tty_check_change(struct tty_struct *tty)
{
	return __tty_check_change(tty, SIGTTOU);
}
