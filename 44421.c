static inline int set_dawr(struct arch_hw_breakpoint *brk)
{
	unsigned long dawr, dawrx, mrd;

	dawr = brk->address;

	dawrx  = (brk->type & (HW_BRK_TYPE_READ | HW_BRK_TYPE_WRITE)) \
		                   << (63 - 58); //* read/write bits */
	dawrx |= ((brk->type & (HW_BRK_TYPE_TRANSLATE)) >> 2) \
		                   << (63 - 59); //* translate */
	dawrx |= (brk->type & (HW_BRK_TYPE_PRIV_ALL)) \
		                   >> 3; //* PRIM bits */
	/* dawr length is stored in field MDR bits 48:53.  Matches range in
	   doublewords (64 bits) baised by -1 eg. 0b000000=1DW and
	   0b111111=64DW.
	   brk->len is in bytes.
	   This aligns up to double word size, shifts and does the bias.
	*/
	mrd = ((brk->len + 7) >> 3) - 1;
	dawrx |= (mrd & 0x3f) << (63 - 53);

	if (ppc_md.set_dawr)
		return ppc_md.set_dawr(dawr, dawrx);
	mtspr(SPRN_DAWR, dawr);
	mtspr(SPRN_DAWRX, dawrx);
	return 0;
}
