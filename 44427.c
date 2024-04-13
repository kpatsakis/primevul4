void show_stack(struct task_struct *tsk, unsigned long *stack)
{
	unsigned long sp, ip, lr, newsp;
	int count = 0;
	int firstframe = 1;
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
	int curr_frame = current->curr_ret_stack;
	extern void return_to_handler(void);
	unsigned long rth = (unsigned long)return_to_handler;
	unsigned long mrth = -1;
#ifdef CONFIG_PPC64
	extern void mod_return_to_handler(void);
	rth = *(unsigned long *)rth;
	mrth = (unsigned long)mod_return_to_handler;
	mrth = *(unsigned long *)mrth;
#endif
#endif

	sp = (unsigned long) stack;
	if (tsk == NULL)
		tsk = current;
	if (sp == 0) {
		if (tsk == current)
			asm("mr %0,1" : "=r" (sp));
		else
			sp = tsk->thread.ksp;
	}

	lr = 0;
	printk("Call Trace:\n");
	do {
		if (!validate_sp(sp, tsk, STACK_FRAME_OVERHEAD))
			return;

		stack = (unsigned long *) sp;
		newsp = stack[0];
		ip = stack[STACK_FRAME_LR_SAVE];
		if (!firstframe || ip != lr) {
			printk("["REG"] ["REG"] %pS", sp, ip, (void *)ip);
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
			if ((ip == rth || ip == mrth) && curr_frame >= 0) {
				printk(" (%pS)",
				       (void *)current->ret_stack[curr_frame].ret);
				curr_frame--;
			}
#endif
			if (firstframe)
				printk(" (unreliable)");
			printk("\n");
		}
		firstframe = 0;

		/*
		 * See if this is an exception frame.
		 * We look for the "regshere" marker in the current frame.
		 */
		if (validate_sp(sp, tsk, STACK_INT_FRAME_SIZE)
		    && stack[STACK_FRAME_MARKER] == STACK_FRAME_REGS_MARKER) {
			struct pt_regs *regs = (struct pt_regs *)
				(sp + STACK_FRAME_OVERHEAD);
			lr = regs->link;
			printk("--- Exception: %lx at %pS\n    LR = %pS\n",
			       regs->trap, (void *)regs->nip, (void *)lr);
			firstframe = 1;
		}

		sp = newsp;
	} while (count++ < kstack_depth_to_print);
}
