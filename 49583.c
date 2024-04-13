static int deflate_comp_init(struct deflate_ctx *ctx)
{
	int ret = 0;
	struct z_stream_s *stream = &ctx->comp_stream;

	stream->workspace = vzalloc(zlib_deflate_workspacesize(
				-DEFLATE_DEF_WINBITS, DEFLATE_DEF_MEMLEVEL));
	if (!stream->workspace) {
		ret = -ENOMEM;
		goto out;
	}
	ret = zlib_deflateInit2(stream, DEFLATE_DEF_LEVEL, Z_DEFLATED,
	                        -DEFLATE_DEF_WINBITS, DEFLATE_DEF_MEMLEVEL,
	                        Z_DEFAULT_STRATEGY);
	if (ret != Z_OK) {
		ret = -EINVAL;
		goto out_free;
	}
out:
	return ret;
out_free:
	vfree(stream->workspace);
	goto out;
}
