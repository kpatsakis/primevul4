void set_personality_ia32(bool x32)
{
	/* inherit personality from parent */

	/* Make sure to be in 32bit mode */
	set_thread_flag(TIF_ADDR32);

	/* Mark the associated mm as containing 32-bit tasks. */
	if (x32) {
		clear_thread_flag(TIF_IA32);
		set_thread_flag(TIF_X32);
		if (current->mm)
			current->mm->context.ia32_compat = TIF_X32;
		current->personality &= ~READ_IMPLIES_EXEC;
		/* is_compat_task() uses the presence of the x32
		   syscall bit flag to determine compat status */
		current_thread_info()->status &= ~TS_COMPAT;
	} else {
		set_thread_flag(TIF_IA32);
		clear_thread_flag(TIF_X32);
		if (current->mm)
			current->mm->context.ia32_compat = TIF_IA32;
		current->personality |= force_personality32;
		/* Prepare the first "return" to user space */
		current_thread_info()->status |= TS_COMPAT;
	}
}
