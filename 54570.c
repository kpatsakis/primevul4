static unsigned long *alloc_thread_stack_node(struct task_struct *tsk,
						  int node)
{
	return kmem_cache_alloc_node(thread_stack_cache, THREADINFO_GFP, node);
}
