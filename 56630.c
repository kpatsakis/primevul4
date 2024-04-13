static unsigned long segment_base(u16 selector)
{
	struct desc_struct *table;
	unsigned long v;

	if (!(selector & ~SEGMENT_RPL_MASK))
		return 0;

	table = get_current_gdt_ro();

	if ((selector & SEGMENT_TI_MASK) == SEGMENT_LDT) {
		u16 ldt_selector = kvm_read_ldt();

		if (!(ldt_selector & ~SEGMENT_RPL_MASK))
			return 0;

		table = (struct desc_struct *)segment_base(ldt_selector);
	}
	v = get_desc_base(&table[selector >> 3]);
	return v;
}
