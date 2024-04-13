void do_SAK(struct tty_struct *tty)
{
	if (!tty)
		return;
	schedule_work(&tty->SAK_work);
}
