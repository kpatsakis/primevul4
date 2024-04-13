static void do_tty_hangup(struct work_struct *work)
{
	struct tty_struct *tty =
		container_of(work, struct tty_struct, hangup_work);

	__tty_hangup(tty);
}
