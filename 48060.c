static int check_dr_read(struct x86_emulate_ctxt *ctxt)
{
	int dr = ctxt->modrm_reg;
	u64 cr4;

	if (dr > 7)
		return emulate_ud(ctxt);

	cr4 = ctxt->ops->get_cr(ctxt, 4);
	if ((cr4 & X86_CR4_DE) && (dr == 4 || dr == 5))
		return emulate_ud(ctxt);

	if (check_dr7_gd(ctxt)) {
		ulong dr6;

		ctxt->ops->get_dr(ctxt, 6, &dr6);
		dr6 &= ~15;
		dr6 |= DR6_BD | DR6_RTM;
		ctxt->ops->set_dr(ctxt, 6, dr6);
		return emulate_db(ctxt);
	}

	return X86EMUL_CONTINUE;
}
