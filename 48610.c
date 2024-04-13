static u64 vhash(unsigned char m[], unsigned int mbytes,
			u64 *tagl, struct vmac_ctx *ctx)
{
	u64 rh, rl, *mptr;
	const u64 *kptr = (u64 *)ctx->nhkey;
	int i, remaining;
	u64 ch, cl;
	u64 pkh = ctx->polykey[0];
	u64 pkl = ctx->polykey[1];

	mptr = (u64 *)m;
	i = mbytes / VMAC_NHBYTES;
	remaining = mbytes % VMAC_NHBYTES;

	if (ctx->first_block_processed) {
		ch = ctx->polytmp[0];
		cl = ctx->polytmp[1];
	} else if (i) {
		nh_vmac_nhbytes(mptr, kptr, VMAC_NHBYTES/8, ch, cl);
		ch &= m62;
		ADD128(ch, cl, pkh, pkl);
		mptr += (VMAC_NHBYTES/sizeof(u64));
		i--;
	} else if (remaining) {
		nh_16(mptr, kptr, 2*((remaining+15)/16), ch, cl);
		ch &= m62;
		ADD128(ch, cl, pkh, pkl);
		mptr += (VMAC_NHBYTES/sizeof(u64));
		goto do_l3;
	} else {/* Empty String */
		ch = pkh; cl = pkl;
		goto do_l3;
	}

	while (i--) {
		nh_vmac_nhbytes(mptr, kptr, VMAC_NHBYTES/8, rh, rl);
		rh &= m62;
		poly_step(ch, cl, pkh, pkl, rh, rl);
		mptr += (VMAC_NHBYTES/sizeof(u64));
	}
	if (remaining) {
		nh_16(mptr, kptr, 2*((remaining+15)/16), rh, rl);
		rh &= m62;
		poly_step(ch, cl, pkh, pkl, rh, rl);
	}

do_l3:
	vhash_abort(ctx);
	remaining *= 8;
	return l3hash(ch, cl, ctx->l3key[0], ctx->l3key[1], remaining);
}
