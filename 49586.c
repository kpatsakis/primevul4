static int deflate_decomp_init(struct deflate_ctx *ctx)
{
	int ret = 0;
	struct z_stream_s *stream = &ctx->decomp_stream;

	stream->workspace = vzalloc(zlib_inflate_workspacesize());
	if (!stream->workspace) {
		ret = -ENOMEM;
		goto out;
	}
	ret = zlib_inflateInit2(stream, -DEFLATE_DEF_WINBITS);
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
