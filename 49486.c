static int get_prng_bytes(char *buf, size_t nbytes, struct prng_context *ctx,
				int do_cont_test)
{
	unsigned char *ptr = buf;
	unsigned int byte_count = (unsigned int)nbytes;
	int err;


	spin_lock_bh(&ctx->prng_lock);

	err = -EINVAL;
	if (ctx->flags & PRNG_NEED_RESET)
		goto done;

	/*
	 * If the FIXED_SIZE flag is on, only return whole blocks of
	 * pseudo random data
	 */
	err = -EINVAL;
	if (ctx->flags & PRNG_FIXED_SIZE) {
		if (nbytes < DEFAULT_BLK_SZ)
			goto done;
		byte_count = DEFAULT_BLK_SZ;
	}

	err = byte_count;

	dbgprint(KERN_CRIT "getting %d random bytes for context %p\n",
		byte_count, ctx);


remainder:
	if (ctx->rand_data_valid == DEFAULT_BLK_SZ) {
		if (_get_more_prng_bytes(ctx, do_cont_test) < 0) {
			memset(buf, 0, nbytes);
			err = -EINVAL;
			goto done;
		}
	}

	/*
	 * Copy any data less than an entire block
	 */
	if (byte_count < DEFAULT_BLK_SZ) {
empty_rbuf:
		while (ctx->rand_data_valid < DEFAULT_BLK_SZ) {
			*ptr = ctx->rand_data[ctx->rand_data_valid];
			ptr++;
			byte_count--;
			ctx->rand_data_valid++;
			if (byte_count == 0)
				goto done;
		}
	}

	/*
	 * Now copy whole blocks
	 */
	for (; byte_count >= DEFAULT_BLK_SZ; byte_count -= DEFAULT_BLK_SZ) {
		if (ctx->rand_data_valid == DEFAULT_BLK_SZ) {
			if (_get_more_prng_bytes(ctx, do_cont_test) < 0) {
				memset(buf, 0, nbytes);
				err = -EINVAL;
				goto done;
			}
		}
		if (ctx->rand_data_valid > 0)
			goto empty_rbuf;
		memcpy(ptr, ctx->rand_data, DEFAULT_BLK_SZ);
		ctx->rand_data_valid += DEFAULT_BLK_SZ;
		ptr += DEFAULT_BLK_SZ;
	}

	/*
	 * Now go back and get any remaining partial block
	 */
	if (byte_count)
		goto remainder;

done:
	spin_unlock_bh(&ctx->prng_lock);
	dbgprint(KERN_CRIT "returning %d from get_prng_bytes in context %p\n",
		err, ctx);
	return err;
}
