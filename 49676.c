static void salsa20_encrypt_bytes(struct salsa20_ctx *ctx, u8 *dst,
				  const u8 *src, unsigned int bytes)
{
	u8 buf[64];

	if (dst != src)
		memcpy(dst, src, bytes);

	while (bytes) {
		salsa20_wordtobyte(buf, ctx->input);

		ctx->input[8]++;
		if (!ctx->input[8])
			ctx->input[9]++;

		if (bytes <= 64) {
			crypto_xor(dst, buf, bytes);
			return;
		}

		crypto_xor(dst, buf, 64);
		bytes -= 64;
		dst += 64;
	}
}
