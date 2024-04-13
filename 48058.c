static inline int assign_eip_near(struct x86_emulate_ctxt *ctxt, ulong dst)
{
	return assign_eip(ctxt, dst, ctxt->mode);
}
