static int em_lgdt_lidt(struct x86_emulate_ctxt *ctxt, bool lgdt)
{
	struct desc_ptr desc_ptr;
	int rc;

	if (ctxt->mode == X86EMUL_MODE_PROT64)
		ctxt->op_bytes = 8;
	rc = read_descriptor(ctxt, ctxt->src.addr.mem,
			     &desc_ptr.size, &desc_ptr.address,
			     ctxt->op_bytes);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	if (ctxt->mode == X86EMUL_MODE_PROT64 &&
	    is_noncanonical_address(desc_ptr.address))
		return emulate_gp(ctxt, 0);
	if (lgdt)
		ctxt->ops->set_gdt(ctxt, &desc_ptr);
	else
		ctxt->ops->set_idt(ctxt, &desc_ptr);
	/* Disable writeback. */
	ctxt->dst.type = OP_NONE;
	return X86EMUL_CONTINUE;
}
