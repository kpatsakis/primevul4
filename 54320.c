void no_tty(void)
{
	struct task_struct *tsk = current;
	tty_lock();
	disassociate_ctty(0);
	tty_unlock();
	proc_clear_tty(tsk);
}
