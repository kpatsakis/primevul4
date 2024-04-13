static void perf_callchain_user_32(struct perf_callchain_entry *entry,
				   struct pt_regs *regs)
{
	unsigned int sp, next_sp;
	unsigned int next_ip;
	unsigned int lr;
	long level = 0;
	unsigned int __user *fp, *uregs;

	next_ip = perf_instruction_pointer(regs);
	lr = regs->link;
	sp = regs->gpr[1];
	perf_callchain_store(entry, next_ip);

	while (entry->nr < PERF_MAX_STACK_DEPTH) {
		fp = (unsigned int __user *) (unsigned long) sp;
		if (!valid_user_sp(sp, 0) || read_user_stack_32(fp, &next_sp))
			return;
		if (level > 0 && read_user_stack_32(&fp[1], &next_ip))
			return;

		uregs = signal_frame_32_regs(sp, next_sp, next_ip);
		if (!uregs && level <= 1)
			uregs = signal_frame_32_regs(sp, next_sp, lr);
		if (uregs) {
			/*
			 * This looks like an signal frame, so restart
			 * the stack trace with the values in it.
			 */
			if (read_user_stack_32(&uregs[PT_NIP], &next_ip) ||
			    read_user_stack_32(&uregs[PT_LNK], &lr) ||
			    read_user_stack_32(&uregs[PT_R1], &sp))
				return;
			level = 0;
			perf_callchain_store(entry, PERF_CONTEXT_USER);
			perf_callchain_store(entry, next_ip);
			continue;
		}

		if (level == 0)
			next_ip = lr;
		perf_callchain_store(entry, next_ip);
		++level;
		sp = next_sp;
	}
}
