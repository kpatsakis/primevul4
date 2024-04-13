void enable_kernel_altivec(void)
{
	WARN_ON(preemptible());

#ifdef CONFIG_SMP
	if (current->thread.regs && (current->thread.regs->msr & MSR_VEC))
		giveup_altivec_maybe_transactional(current);
	else
		giveup_altivec_notask();
#else
	giveup_altivec_maybe_transactional(last_task_used_altivec);
#endif /* CONFIG_SMP */
}
