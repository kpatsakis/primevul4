void switch_booke_debug_regs(struct debug_reg *new_debug)
{
	if ((current->thread.debug.dbcr0 & DBCR0_IDM)
		|| (new_debug->dbcr0 & DBCR0_IDM))
			prime_debug_regs(new_debug);
}
