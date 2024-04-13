static inline bool hw_brk_match(struct arch_hw_breakpoint *a,
			      struct arch_hw_breakpoint *b)
{
	if (a->address != b->address)
		return false;
	if (a->type != b->type)
		return false;
	if (a->len != b->len)
		return false;
	return true;
}
