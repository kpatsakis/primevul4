static void zlib_comp_exit(struct zlib_ctx *ctx)
{
	struct z_stream_s *stream = &ctx->comp_stream;

	if (stream->workspace) {
		zlib_deflateEnd(stream);
		vfree(stream->workspace);
		stream->workspace = NULL;
	}
}
