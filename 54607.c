static void release_task_stack(struct task_struct *tsk)
{
	if (WARN_ON(tsk->state != TASK_DEAD))
		return;  /* Better to leak the stack than to free prematurely */

	account_kernel_stack(tsk, -1);
	arch_release_thread_stack(tsk->stack);
	free_thread_stack(tsk);
	tsk->stack = NULL;
#ifdef CONFIG_VMAP_STACK
	tsk->stack_vm_area = NULL;
#endif
}
