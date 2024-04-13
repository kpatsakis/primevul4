static int rmd320_init(struct shash_desc *desc)
{
	struct rmd320_ctx *rctx = shash_desc_ctx(desc);

	rctx->byte_count = 0;

	rctx->state[0] = RMD_H0;
	rctx->state[1] = RMD_H1;
	rctx->state[2] = RMD_H2;
	rctx->state[3] = RMD_H3;
	rctx->state[4] = RMD_H4;
	rctx->state[5] = RMD_H5;
	rctx->state[6] = RMD_H6;
	rctx->state[7] = RMD_H7;
	rctx->state[8] = RMD_H8;
	rctx->state[9] = RMD_H9;

	memset(rctx->buffer, 0, sizeof(rctx->buffer));

	return 0;
}
