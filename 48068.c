static int em_lidt(struct x86_emulate_ctxt *ctxt)
{
	return em_lgdt_lidt(ctxt, false);
}
