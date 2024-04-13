void __flush_signals(struct task_struct *t)
{
	clear_tsk_thread_flag(t, TIF_SIGPENDING);
	flush_sigqueue(&t->pending);
	flush_sigqueue(&t->signal->shared_pending);
}
