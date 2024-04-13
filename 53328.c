static void queue_release_one_tty(struct kref *kref)
{
	struct tty_struct *tty = container_of(kref, struct tty_struct, kref);

	/* The hangup queue is now free so we can reuse it rather than
	   waste a chunk of memory for each port */
	INIT_WORK(&tty->hangup_work, release_one_tty);
	schedule_work(&tty->hangup_work);
}
