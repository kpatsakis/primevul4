static int zlib_compress_setup(struct crypto_pcomp *tfm, void *params,
			       unsigned int len)
{
	struct zlib_ctx *ctx = crypto_tfm_ctx(crypto_pcomp_tfm(tfm));
	struct z_stream_s *stream = &ctx->comp_stream;
	struct nlattr *tb[ZLIB_COMP_MAX + 1];
	int window_bits, mem_level;
	size_t workspacesize;
	int ret;

	ret = nla_parse(tb, ZLIB_COMP_MAX, params, len, NULL);
	if (ret)
		return ret;

	zlib_comp_exit(ctx);

	window_bits = tb[ZLIB_COMP_WINDOWBITS]
					? nla_get_u32(tb[ZLIB_COMP_WINDOWBITS])
					: MAX_WBITS;
	mem_level = tb[ZLIB_COMP_MEMLEVEL]
					? nla_get_u32(tb[ZLIB_COMP_MEMLEVEL])
					: DEF_MEM_LEVEL;

	workspacesize = zlib_deflate_workspacesize(window_bits, mem_level);
	stream->workspace = vzalloc(workspacesize);
	if (!stream->workspace)
		return -ENOMEM;

	ret = zlib_deflateInit2(stream,
				tb[ZLIB_COMP_LEVEL]
					? nla_get_u32(tb[ZLIB_COMP_LEVEL])
					: Z_DEFAULT_COMPRESSION,
				tb[ZLIB_COMP_METHOD]
					? nla_get_u32(tb[ZLIB_COMP_METHOD])
					: Z_DEFLATED,
				window_bits,
				mem_level,
				tb[ZLIB_COMP_STRATEGY]
					? nla_get_u32(tb[ZLIB_COMP_STRATEGY])
					: Z_DEFAULT_STRATEGY);
	if (ret != Z_OK) {
		vfree(stream->workspace);
		stream->workspace = NULL;
		return -EINVAL;
	}

	return 0;
}
