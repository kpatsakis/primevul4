static void free_thread_stack(struct task_struct *tsk)
{
	kmem_cache_free(thread_stack_cache, tsk->stack);
}
