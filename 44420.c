int set_breakpoint(struct arch_hw_breakpoint *brk)
{
	__get_cpu_var(current_brk) = *brk;

	if (cpu_has_feature(CPU_FTR_DAWR))
		return set_dawr(brk);

	return set_dabr(brk);
}
