static int padlock_sha256_update_nano(struct shash_desc *desc, const u8 *data,
			  unsigned int len)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	unsigned int partial, done;
	const u8 *src;
	/*The PHE require the out buffer must 128 bytes and 16-bytes aligned*/
	u8 buf[128 + PADLOCK_ALIGNMENT - STACK_ALIGN] __attribute__
		((aligned(STACK_ALIGN)));
	u8 *dst = PTR_ALIGN(&buf[0], PADLOCK_ALIGNMENT);
	int ts_state;

	partial = sctx->count & 0x3f;
	sctx->count += len;
	done = 0;
	src = data;
	memcpy(dst, (u8 *)(sctx->state), SHA256_DIGEST_SIZE);

	if ((partial + len) >= SHA256_BLOCK_SIZE) {

		/* Append the bytes in state's buffer to a block to handle */
		if (partial) {
			done = -partial;
			memcpy(sctx->buf + partial, data,
				done + SHA256_BLOCK_SIZE);
			src = sctx->buf;
			ts_state = irq_ts_save();
			asm volatile (".byte 0xf3,0x0f,0xa6,0xd0"
			: "+S"(src), "+D"(dst)
			: "a"((long)-1), "c"((unsigned long)1));
			irq_ts_restore(ts_state);
			done += SHA256_BLOCK_SIZE;
			src = data + done;
		}

		/* Process the left bytes from input data*/
		if (len - done >= SHA256_BLOCK_SIZE) {
			ts_state = irq_ts_save();
			asm volatile (".byte 0xf3,0x0f,0xa6,0xd0"
			: "+S"(src), "+D"(dst)
			: "a"((long)-1),
			"c"((unsigned long)((len - done) / 64)));
			irq_ts_restore(ts_state);
			done += ((len - done) - (len - done) % 64);
			src = data + done;
		}
		partial = 0;
	}
	memcpy((u8 *)(sctx->state), dst, SHA256_DIGEST_SIZE);
	memcpy(sctx->buf + partial, src, len - done);

	return 0;
}
