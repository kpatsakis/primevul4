static int em_lgdt(struct x86_emulate_ctxt *ctxt)
{
	return em_lgdt_lidt(ctxt, true);
}
