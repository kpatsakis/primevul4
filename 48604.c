static void seqiv_geniv(struct seqiv_ctx *ctx, u8 *info, u64 seq,
			unsigned int ivsize)
{
	unsigned int len = ivsize;

	if (ivsize > sizeof(u64)) {
		memset(info, 0, ivsize - sizeof(u64));
		len = sizeof(u64);
	}
	seq = cpu_to_be64(seq);
	memcpy(info + ivsize - len, &seq, len);
	crypto_xor(info, ctx->salt, ivsize);
}
