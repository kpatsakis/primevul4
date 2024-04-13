static inline int valid_irq_stack(unsigned long sp, struct task_struct *p,
				  unsigned long nbytes)
{
	unsigned long stack_page;
	unsigned long cpu = task_cpu(p);

	/*
	 * Avoid crashing if the stack has overflowed and corrupted
	 * task_cpu(p), which is in the thread_info struct.
	 */
	if (cpu < NR_CPUS && cpu_possible(cpu)) {
		stack_page = (unsigned long) hardirq_ctx[cpu];
		if (sp >= stack_page + sizeof(struct thread_struct)
		    && sp <= stack_page + THREAD_SIZE - nbytes)
			return 1;

		stack_page = (unsigned long) softirq_ctx[cpu];
		if (sp >= stack_page + sizeof(struct thread_struct)
		    && sp <= stack_page + THREAD_SIZE - nbytes)
			return 1;
	}
	return 0;
}
