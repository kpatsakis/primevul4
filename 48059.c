static int check_cr_write(struct x86_emulate_ctxt *ctxt)
{
	u64 new_val = ctxt->src.val64;
	int cr = ctxt->modrm_reg;
	u64 efer = 0;

	static u64 cr_reserved_bits[] = {
		0xffffffff00000000ULL,
		0, 0, 0, /* CR3 checked later */
		CR4_RESERVED_BITS,
		0, 0, 0,
		CR8_RESERVED_BITS,
	};

	if (!valid_cr(cr))
		return emulate_ud(ctxt);

	if (new_val & cr_reserved_bits[cr])
		return emulate_gp(ctxt, 0);

	switch (cr) {
	case 0: {
		u64 cr4;
		if (((new_val & X86_CR0_PG) && !(new_val & X86_CR0_PE)) ||
		    ((new_val & X86_CR0_NW) && !(new_val & X86_CR0_CD)))
			return emulate_gp(ctxt, 0);

		cr4 = ctxt->ops->get_cr(ctxt, 4);
		ctxt->ops->get_msr(ctxt, MSR_EFER, &efer);

		if ((new_val & X86_CR0_PG) && (efer & EFER_LME) &&
		    !(cr4 & X86_CR4_PAE))
			return emulate_gp(ctxt, 0);

		break;
		}
	case 3: {
		u64 rsvd = 0;

		ctxt->ops->get_msr(ctxt, MSR_EFER, &efer);
		if (efer & EFER_LMA)
			rsvd = CR3_L_MODE_RESERVED_BITS & ~CR3_PCID_INVD;

		if (new_val & rsvd)
			return emulate_gp(ctxt, 0);

		break;
		}
	case 4: {
		ctxt->ops->get_msr(ctxt, MSR_EFER, &efer);

		if ((efer & EFER_LMA) && !(new_val & X86_CR4_PAE))
			return emulate_gp(ctxt, 0);

		break;
		}
	}

	return X86EMUL_CONTINUE;
}
