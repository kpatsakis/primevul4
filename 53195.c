trad_enc_decypt_byte(struct trad_enc_ctx *ctx)
{
	unsigned temp = ctx->keys[2] | 2;
	return (uint8_t)((temp * (temp ^ 1)) >> 8) & 0xff;
}
