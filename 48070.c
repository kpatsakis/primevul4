static int em_mov_rm_sreg(struct x86_emulate_ctxt *ctxt)
{
	if (ctxt->modrm_reg > VCPU_SREG_GS)
		return emulate_ud(ctxt);

	ctxt->dst.val = get_segment_selector(ctxt, ctxt->modrm_reg);
	if (ctxt->dst.bytes == 4 && ctxt->dst.type == OP_MEM)
		ctxt->dst.bytes = 2;
	return X86EMUL_CONTINUE;
}
