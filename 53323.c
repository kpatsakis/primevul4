void no_tty(void)
{
	/* FIXME: Review locking here. The tty_lock never covered any race
	   between a new association and proc_clear_tty but possible we need
	   to protect against this anyway */
	struct task_struct *tsk = current;
	disassociate_ctty(0);
	proc_clear_tty(tsk);
}
