static int assign_eip_far(struct x86_emulate_ctxt *ctxt, ulong dst,
			  const struct desc_struct *cs_desc)
{
	enum x86emul_mode mode = ctxt->mode;

#ifdef CONFIG_X86_64
	if (ctxt->mode >= X86EMUL_MODE_PROT32 && cs_desc->l) {
		u64 efer = 0;

		ctxt->ops->get_msr(ctxt, MSR_EFER, &efer);
		if (efer & EFER_LMA)
			mode = X86EMUL_MODE_PROT64;
	}
#endif
	if (mode == X86EMUL_MODE_PROT16 || mode == X86EMUL_MODE_PROT32)
		mode = cs_desc->d ? X86EMUL_MODE_PROT32 : X86EMUL_MODE_PROT16;
	return assign_eip(ctxt, dst, mode);
}
