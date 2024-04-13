void tty_hangup(struct tty_struct *tty)
{
	tty_debug_hangup(tty, "hangup\n");
	schedule_work(&tty->hangup_work);
}
