static int zlib_decompress_setup(struct crypto_pcomp *tfm, void *params,
				 unsigned int len)
{
	struct zlib_ctx *ctx = crypto_tfm_ctx(crypto_pcomp_tfm(tfm));
	struct z_stream_s *stream = &ctx->decomp_stream;
	struct nlattr *tb[ZLIB_DECOMP_MAX + 1];
	int ret = 0;

	ret = nla_parse(tb, ZLIB_DECOMP_MAX, params, len, NULL);
	if (ret)
		return ret;

	zlib_decomp_exit(ctx);

	ctx->decomp_windowBits = tb[ZLIB_DECOMP_WINDOWBITS]
				 ? nla_get_u32(tb[ZLIB_DECOMP_WINDOWBITS])
				 : DEF_WBITS;

	stream->workspace = vzalloc(zlib_inflate_workspacesize());
	if (!stream->workspace)
		return -ENOMEM;

	ret = zlib_inflateInit2(stream, ctx->decomp_windowBits);
	if (ret != Z_OK) {
		vfree(stream->workspace);
		stream->workspace = NULL;
		return -EINVAL;
	}

	return 0;
}
