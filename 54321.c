static void proc_set_tty(struct task_struct *tsk, struct tty_struct *tty)
{
	spin_lock_irq(&tsk->sighand->siglock);
	__proc_set_tty(tsk, tty);
	spin_unlock_irq(&tsk->sighand->siglock);
}
