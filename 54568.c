static void account_kernel_stack(struct task_struct *tsk, int account)
{
	void *stack = task_stack_page(tsk);
	struct vm_struct *vm = task_stack_vm_area(tsk);

	BUILD_BUG_ON(IS_ENABLED(CONFIG_VMAP_STACK) && PAGE_SIZE % 1024 != 0);

	if (vm) {
		int i;

		BUG_ON(vm->nr_pages != THREAD_SIZE / PAGE_SIZE);

		for (i = 0; i < THREAD_SIZE / PAGE_SIZE; i++) {
			mod_zone_page_state(page_zone(vm->pages[i]),
					    NR_KERNEL_STACK_KB,
					    PAGE_SIZE / 1024 * account);
		}

		/* All stack pages belong to the same memcg. */
		mod_memcg_page_state(vm->pages[0], MEMCG_KERNEL_STACK_KB,
				     account * (THREAD_SIZE / 1024));
	} else {
		/*
		 * All stack pages are in the same zone and belong to the
		 * same memcg.
		 */
		struct page *first_page = virt_to_page(stack);

		mod_zone_page_state(page_zone(first_page), NR_KERNEL_STACK_KB,
				    THREAD_SIZE / 1024 * account);

		mod_memcg_page_state(first_page, MEMCG_KERNEL_STACK_KB,
				     account * (THREAD_SIZE / 1024));
	}
}
