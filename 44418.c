unsigned long randomize_et_dyn(unsigned long base)
{
	unsigned long ret = PAGE_ALIGN(base + brk_rnd());

	if (ret < base)
		return base;

	return ret;
}
