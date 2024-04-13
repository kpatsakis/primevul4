void __set_current_blocked(const sigset_t *newset)
{
	struct task_struct *tsk = current;

	spin_lock_irq(&tsk->sighand->siglock);
	__set_task_blocked(tsk, newset);
	spin_unlock_irq(&tsk->sighand->siglock);
}
