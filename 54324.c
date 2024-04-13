static void release_tty(struct tty_struct *tty, int idx)
{
	/* This should always be true but check for the moment */
	WARN_ON(tty->index != idx);

	if (tty->link)
		tty_kref_put(tty->link);
	tty_kref_put(tty);
}
