static int chksum_init(struct shash_desc *desc)
{
	struct chksum_desc_ctx *ctx = shash_desc_ctx(desc);

	ctx->crc = 0;

	return 0;
}
