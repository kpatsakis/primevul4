static int zlib_compress_init(struct crypto_pcomp *tfm)
{
	int ret;
	struct zlib_ctx *dctx = crypto_tfm_ctx(crypto_pcomp_tfm(tfm));
	struct z_stream_s *stream = &dctx->comp_stream;

	ret = zlib_deflateReset(stream);
	if (ret != Z_OK)
		return -EINVAL;

	return 0;
}
