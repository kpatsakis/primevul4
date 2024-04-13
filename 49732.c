static int tgr192_init(struct shash_desc *desc)
{
	struct tgr192_ctx *tctx = shash_desc_ctx(desc);

	tctx->a = 0x0123456789abcdefULL;
	tctx->b = 0xfedcba9876543210ULL;
	tctx->c = 0xf096a5b4c3b2e187ULL;
	tctx->nblocks = 0;
	tctx->count = 0;

	return 0;
}
