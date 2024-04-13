static void ptrace_unfreeze_traced(struct task_struct *task)
{
	if (task->state != __TASK_TRACED)
		return;

	WARN_ON(!task->ptrace || task->parent != current);

	spin_lock_irq(&task->sighand->siglock);
	if (__fatal_signal_pending(task))
		wake_up_state(task, __TASK_TRACED);
	else
		task->state = TASK_TRACED;
	spin_unlock_irq(&task->sighand->siglock);
}
