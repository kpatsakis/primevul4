static void set_debug_reg_defaults(struct thread_struct *thread)
{
	thread->hw_brk.address = 0;
	thread->hw_brk.type = 0;
	set_breakpoint(&thread->hw_brk);
}
