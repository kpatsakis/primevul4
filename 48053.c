static int __do_insn_fetch_bytes(struct x86_emulate_ctxt *ctxt, int op_size)
{
	int rc;
	unsigned size, max_size;
	unsigned long linear;
	int cur_size = ctxt->fetch.end - ctxt->fetch.data;
	struct segmented_address addr = { .seg = VCPU_SREG_CS,
					   .ea = ctxt->eip + cur_size };

	/*
	 * We do not know exactly how many bytes will be needed, and
	 * __linearize is expensive, so fetch as much as possible.  We
	 * just have to avoid going beyond the 15 byte limit, the end
	 * of the segment, or the end of the page.
	 *
	 * __linearize is called with size 0 so that it does not do any
	 * boundary check itself.  Instead, we use max_size to check
	 * against op_size.
	 */
	rc = __linearize(ctxt, addr, &max_size, 0, false, true, ctxt->mode,
			 &linear);
	if (unlikely(rc != X86EMUL_CONTINUE))
		return rc;

	size = min_t(unsigned, 15UL ^ cur_size, max_size);
	size = min_t(unsigned, size, PAGE_SIZE - offset_in_page(linear));

	/*
	 * One instruction can only straddle two pages,
	 * and one has been loaded at the beginning of
	 * x86_decode_insn.  So, if not enough bytes
	 * still, we must have hit the 15-byte boundary.
	 */
	if (unlikely(size < op_size))
		return emulate_gp(ctxt, 0);

	rc = ctxt->ops->fetch(ctxt, linear, ctxt->fetch.end,
			      size, &ctxt->exception);
	if (unlikely(rc != X86EMUL_CONTINUE))
		return rc;
	ctxt->fetch.end += size;
	return X86EMUL_CONTINUE;
}
