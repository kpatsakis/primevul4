perf_callchain_kernel(struct perf_callchain_entry *entry, struct pt_regs *regs)
{
	unsigned long sp, next_sp;
	unsigned long next_ip;
	unsigned long lr;
	long level = 0;
	unsigned long *fp;

	lr = regs->link;
	sp = regs->gpr[1];
	perf_callchain_store(entry, perf_instruction_pointer(regs));

	if (!validate_sp(sp, current, STACK_FRAME_OVERHEAD))
		return;

	for (;;) {
		fp = (unsigned long *) sp;
		next_sp = fp[0];

		if (next_sp == sp + STACK_INT_FRAME_SIZE &&
		    fp[STACK_FRAME_MARKER] == STACK_FRAME_REGS_MARKER) {
			/*
			 * This looks like an interrupt frame for an
			 * interrupt that occurred in the kernel
			 */
			regs = (struct pt_regs *)(sp + STACK_FRAME_OVERHEAD);
			next_ip = regs->nip;
			lr = regs->link;
			level = 0;
			perf_callchain_store(entry, PERF_CONTEXT_KERNEL);

		} else {
			if (level == 0)
				next_ip = lr;
			else
				next_ip = fp[STACK_FRAME_LR_SAVE];

			/*
			 * We can't tell which of the first two addresses
			 * we get are valid, but we can filter out the
			 * obviously bogus ones here.  We replace them
			 * with 0 rather than removing them entirely so
			 * that userspace can tell which is which.
			 */
			if ((level == 1 && next_ip == lr) ||
			    (level <= 1 && !kernel_text_address(next_ip)))
				next_ip = 0;

			++level;
		}

		perf_callchain_store(entry, next_ip);
		if (!valid_next_sp(next_sp, sp))
			return;
		sp = next_sp;
	}
}
