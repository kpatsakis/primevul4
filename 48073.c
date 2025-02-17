static int em_ret_far(struct x86_emulate_ctxt *ctxt)
{
	int rc;
	unsigned long eip, cs;
	u16 old_cs;
	int cpl = ctxt->ops->cpl(ctxt);
	struct desc_struct old_desc, new_desc;
	const struct x86_emulate_ops *ops = ctxt->ops;

	if (ctxt->mode == X86EMUL_MODE_PROT64)
		ops->get_segment(ctxt, &old_cs, &old_desc, NULL,
				 VCPU_SREG_CS);

	rc = emulate_pop(ctxt, &eip, ctxt->op_bytes);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	rc = emulate_pop(ctxt, &cs, ctxt->op_bytes);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	/* Outer-privilege level return is not implemented */
	if (ctxt->mode >= X86EMUL_MODE_PROT16 && (cs & 3) > cpl)
		return X86EMUL_UNHANDLEABLE;
	rc = __load_segment_descriptor(ctxt, (u16)cs, VCPU_SREG_CS, cpl, false,
				       &new_desc);
	if (rc != X86EMUL_CONTINUE)
		return rc;
	rc = assign_eip_far(ctxt, eip, &new_desc);
	if (rc != X86EMUL_CONTINUE) {
		WARN_ON(ctxt->mode != X86EMUL_MODE_PROT64);
		ops->set_segment(ctxt, old_cs, &old_desc, 0, VCPU_SREG_CS);
	}
	return rc;
}
