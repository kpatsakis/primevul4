static int chksum_update(struct shash_desc *desc, const u8 *data,
			 unsigned int length)
{
	struct chksum_desc_ctx *ctx = shash_desc_ctx(desc);

	if (irq_fpu_usable()) {
		kernel_fpu_begin();
		ctx->crc = crc_t10dif_pcl(ctx->crc, data, length);
		kernel_fpu_end();
	} else
		ctx->crc = crc_t10dif_generic(ctx->crc, data, length);
	return 0;
}
