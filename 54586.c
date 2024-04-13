static inline void free_thread_stack(struct task_struct *tsk)
{
#ifdef CONFIG_VMAP_STACK
	if (task_stack_vm_area(tsk)) {
		int i;

		for (i = 0; i < NR_CACHED_STACKS; i++) {
			if (this_cpu_cmpxchg(cached_stacks[i],
					NULL, tsk->stack_vm_area) != NULL)
				continue;

			return;
		}

		vfree_atomic(tsk->stack);
		return;
	}
#endif

	__free_pages(virt_to_page(tsk->stack), THREAD_SIZE_ORDER);
}
