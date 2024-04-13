register_address_increment(struct x86_emulate_ctxt *ctxt, int reg, int inc)
{
	ulong mask;

	if (ctxt->ad_bytes == sizeof(unsigned long))
		mask = ~0UL;
	else
		mask = ad_mask(ctxt);
	masked_increment(reg_rmw(ctxt, reg), mask, inc);
}
