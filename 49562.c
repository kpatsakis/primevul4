static int chksum_final(struct shash_desc *desc, u8 *out)
{
	struct chksum_desc_ctx *ctx = shash_desc_ctx(desc);

	*(__le32 *)out = ~cpu_to_le32p(&ctx->crc);
	return 0;
}
