void __set_breakpoint(struct arch_hw_breakpoint *brk)
{
	memcpy(this_cpu_ptr(&current_brk), brk, sizeof(*brk));

	if (cpu_has_feature(CPU_FTR_DAWR))
		set_dawr(brk);
	else
		set_dabr(brk);
}
