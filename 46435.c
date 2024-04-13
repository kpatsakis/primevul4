static int valid_next_sp(unsigned long sp, unsigned long prev_sp)
{
	if (sp & 0xf)
		return 0;		/* must be 16-byte aligned */
	if (!validate_sp(sp, current, STACK_FRAME_OVERHEAD))
		return 0;
	if (sp >= prev_sp + STACK_FRAME_MIN_SIZE)
		return 1;
	/*
	 * sp could decrease when we jump off an interrupt stack
	 * back to the regular process stack.
	 */
	if ((sp & ~(THREAD_SIZE - 1)) != (prev_sp & ~(THREAD_SIZE - 1)))
		return 1;
	return 0;
}
