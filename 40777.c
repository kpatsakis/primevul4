SYSCALL_DEFINE0(pause)
{
	while (!signal_pending(current)) {
		current->state = TASK_INTERRUPTIBLE;
		schedule();
	}
	return -ERESTARTNOHAND;
}
