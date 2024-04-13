static inline int assign_eip(struct x86_emulate_ctxt *ctxt, ulong dst,
			     enum x86emul_mode mode)
{
	ulong linear;
	int rc;
	unsigned max_size;
	struct segmented_address addr = { .seg = VCPU_SREG_CS,
					   .ea = dst };

	if (ctxt->op_bytes != sizeof(unsigned long))
		addr.ea = dst & ((1UL << (ctxt->op_bytes << 3)) - 1);
	rc = __linearize(ctxt, addr, &max_size, 1, false, true, mode, &linear);
	if (rc == X86EMUL_CONTINUE)
		ctxt->_eip = addr.ea;
	return rc;
}
