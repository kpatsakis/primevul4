static int rmd128_init(struct shash_desc *desc)
{
	struct rmd128_ctx *rctx = shash_desc_ctx(desc);

	rctx->byte_count = 0;

	rctx->state[0] = RMD_H0;
	rctx->state[1] = RMD_H1;
	rctx->state[2] = RMD_H2;
	rctx->state[3] = RMD_H3;

	memset(rctx->buffer, 0, sizeof(rctx->buffer));

	return 0;
}
