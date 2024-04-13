static void vhash_update(const unsigned char *m,
			unsigned int mbytes, /* Pos multiple of VMAC_NHBYTES */
			struct vmac_ctx *ctx)
{
	u64 rh, rl, *mptr;
	const u64 *kptr = (u64 *)ctx->nhkey;
	int i;
	u64 ch, cl;
	u64 pkh = ctx->polykey[0];
	u64 pkl = ctx->polykey[1];

	if (!mbytes)
		return;

	BUG_ON(mbytes % VMAC_NHBYTES);

	mptr = (u64 *)m;
	i = mbytes / VMAC_NHBYTES;  /* Must be non-zero */

	ch = ctx->polytmp[0];
	cl = ctx->polytmp[1];

	if (!ctx->first_block_processed) {
		ctx->first_block_processed = 1;
		nh_vmac_nhbytes(mptr, kptr, VMAC_NHBYTES/8, rh, rl);
		rh &= m62;
		ADD128(ch, cl, rh, rl);
		mptr += (VMAC_NHBYTES/sizeof(u64));
		i--;
	}

	while (i--) {
		nh_vmac_nhbytes(mptr, kptr, VMAC_NHBYTES/8, rh, rl);
		rh &= m62;
		poly_step(ch, cl, pkh, pkl, rh, rl);
		mptr += (VMAC_NHBYTES/sizeof(u64));
	}

	ctx->polytmp[0] = ch;
	ctx->polytmp[1] = cl;
}
