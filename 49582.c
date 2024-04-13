static void deflate_comp_exit(struct deflate_ctx *ctx)
{
	zlib_deflateEnd(&ctx->comp_stream);
	vfree(ctx->comp_stream.workspace);
}
