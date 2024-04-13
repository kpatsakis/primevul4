static void zlib_decomp_exit(struct zlib_ctx *ctx)
{
	struct z_stream_s *stream = &ctx->decomp_stream;

	if (stream->workspace) {
		zlib_inflateEnd(stream);
		vfree(stream->workspace);
		stream->workspace = NULL;
	}
}
