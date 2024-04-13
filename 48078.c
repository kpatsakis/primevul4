register_address(struct x86_emulate_ctxt *ctxt, int reg)
{
	return address_mask(ctxt, reg_read(ctxt, reg));
}
