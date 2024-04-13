static void deflate_decomp_exit(struct deflate_ctx *ctx)
{
	zlib_inflateEnd(&ctx->decomp_stream);
	vfree(ctx->decomp_stream.workspace);
}
