asmlinkage void __sched schedule(void)
{
	struct task_struct *tsk = current;

	sched_submit_work(tsk);
	__schedule();
}
