static int free_vm_stack_cache(unsigned int cpu)
{
	struct vm_struct **cached_vm_stacks = per_cpu_ptr(cached_stacks, cpu);
	int i;

	for (i = 0; i < NR_CACHED_STACKS; i++) {
		struct vm_struct *vm_stack = cached_vm_stacks[i];

		if (!vm_stack)
			continue;

		vfree(vm_stack->addr);
		cached_vm_stacks[i] = NULL;
	}

	return 0;
}
