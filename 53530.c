void enable_kernel_vsx(void)
{
	WARN_ON(preemptible());

#ifdef CONFIG_SMP
	if (current->thread.regs && (current->thread.regs->msr & MSR_VSX))
		giveup_vsx(current);
	else
		giveup_vsx(NULL);	/* just enable vsx for kernel - force */
#else
	giveup_vsx(last_task_used_vsx);
#endif /* CONFIG_SMP */
}
