static __always_inline int __linearize(struct x86_emulate_ctxt *ctxt,
				       struct segmented_address addr,
				       unsigned *max_size, unsigned size,
				       bool write, bool fetch,
				       enum x86emul_mode mode, ulong *linear)
{
	struct desc_struct desc;
	bool usable;
	ulong la;
	u32 lim;
	u16 sel;

	la = seg_base(ctxt, addr.seg) + addr.ea;
	*max_size = 0;
	switch (mode) {
	case X86EMUL_MODE_PROT64:
		if (is_noncanonical_address(la))
			goto bad;

		*max_size = min_t(u64, ~0u, (1ull << 48) - la);
		if (size > *max_size)
			goto bad;
		break;
	default:
		usable = ctxt->ops->get_segment(ctxt, &sel, &desc, NULL,
						addr.seg);
		if (!usable)
			goto bad;
		/* code segment in protected mode or read-only data segment */
		if ((((ctxt->mode != X86EMUL_MODE_REAL) && (desc.type & 8))
					|| !(desc.type & 2)) && write)
			goto bad;
		/* unreadable code segment */
		if (!fetch && (desc.type & 8) && !(desc.type & 2))
			goto bad;
		lim = desc_limit_scaled(&desc);
		if (!(desc.type & 8) && (desc.type & 4)) {
			/* expand-down segment */
			if (addr.ea <= lim)
				goto bad;
			lim = desc.d ? 0xffffffff : 0xffff;
		}
		if (addr.ea > lim)
			goto bad;
		*max_size = min_t(u64, ~0u, (u64)lim + 1 - addr.ea);
		if (size > *max_size)
			goto bad;
		la &= (u32)-1;
		break;
	}
	if (insn_aligned(ctxt, size) && ((la & (size - 1)) != 0))
		return emulate_gp(ctxt, 0);
	*linear = la;
	return X86EMUL_CONTINUE;
bad:
	if (addr.seg == VCPU_SREG_SS)
		return emulate_ss(ctxt, 0);
	else
		return emulate_gp(ctxt, 0);
}
