void set_personality_64bit(void)
{
	/* inherit personality from parent */

	/* Make sure to be in 64bit mode */
	clear_thread_flag(TIF_IA32);
	clear_thread_flag(TIF_ADDR32);
	clear_thread_flag(TIF_X32);

	/* Ensure the corresponding mm is not marked. */
	if (current->mm)
		current->mm->context.ia32_compat = 0;

	/* TBD: overwrites user setup. Should have two bits.
	   But 64bit processes have always behaved this way,
	   so it's not too bad. The main problem is just that
	   32bit childs are affected again. */
	current->personality &= ~READ_IMPLIES_EXEC;
}
