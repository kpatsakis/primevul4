trad_enc_update_keys(struct trad_enc_ctx *ctx, uint8_t c)
{
	uint8_t t;
#define CRC32(c, b) (crc32(c ^ 0xffffffffUL, &b, 1) ^ 0xffffffffUL)

	ctx->keys[0] = CRC32(ctx->keys[0], c);
	ctx->keys[1] = (ctx->keys[1] + (ctx->keys[0] & 0xff)) * 134775813L + 1;
	t = (ctx->keys[1] >> 24) & 0xff;
	ctx->keys[2] = CRC32(ctx->keys[2], t);
#undef CRC32
}
