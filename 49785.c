static int padlock_sha1_finup(struct shash_desc *desc, const u8 *in,
			      unsigned int count, u8 *out)
{
	/* We can't store directly to *out as it may be unaligned. */
	/* BTW Don't reduce the buffer size below 128 Bytes!
	 *     PadLock microcode needs it that big. */
	char buf[128 + PADLOCK_ALIGNMENT - STACK_ALIGN] __attribute__
		((aligned(STACK_ALIGN)));
	char *result = PTR_ALIGN(&buf[0], PADLOCK_ALIGNMENT);
	struct padlock_sha_desc *dctx = shash_desc_ctx(desc);
	struct sha1_state state;
	unsigned int space;
	unsigned int leftover;
	int ts_state;
	int err;

	dctx->fallback.flags = desc->flags & CRYPTO_TFM_REQ_MAY_SLEEP;
	err = crypto_shash_export(&dctx->fallback, &state);
	if (err)
		goto out;

	if (state.count + count > ULONG_MAX)
		return crypto_shash_finup(&dctx->fallback, in, count, out);

	leftover = ((state.count - 1) & (SHA1_BLOCK_SIZE - 1)) + 1;
	space =  SHA1_BLOCK_SIZE - leftover;
	if (space) {
		if (count > space) {
			err = crypto_shash_update(&dctx->fallback, in, space) ?:
			      crypto_shash_export(&dctx->fallback, &state);
			if (err)
				goto out;
			count -= space;
			in += space;
		} else {
			memcpy(state.buffer + leftover, in, count);
			in = state.buffer;
			count += leftover;
			state.count &= ~(SHA1_BLOCK_SIZE - 1);
		}
	}

	memcpy(result, &state.state, SHA1_DIGEST_SIZE);

	/* prevent taking the spurious DNA fault with padlock. */
	ts_state = irq_ts_save();
	asm volatile (".byte 0xf3,0x0f,0xa6,0xc8" /* rep xsha1 */
		      : \
		      : "c"((unsigned long)state.count + count), \
			"a"((unsigned long)state.count), \
			"S"(in), "D"(result));
	irq_ts_restore(ts_state);

	padlock_output_block((uint32_t *)result, (uint32_t *)out, 5);

out:
	return err;
}
