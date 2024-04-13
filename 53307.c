void deinitialize_tty_struct(struct tty_struct *tty)
{
	tty_ldisc_deinit(tty);
}
