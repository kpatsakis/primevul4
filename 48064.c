static int em_jmp_abs(struct x86_emulate_ctxt *ctxt)
{
	return assign_eip_near(ctxt, ctxt->src.val);
}
