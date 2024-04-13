void put_task_stack(struct task_struct *tsk)
{
	if (atomic_dec_and_test(&tsk->stack_refcount))
		release_task_stack(tsk);
}
