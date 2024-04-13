static void tty_flush_works(struct tty_struct *tty)
{
	flush_work(&tty->SAK_work);
	flush_work(&tty->hangup_work);
	if (tty->link) {
		flush_work(&tty->link->SAK_work);
		flush_work(&tty->link->hangup_work);
	}
}
