IW_IMPL(void) iw_set_random_seed(struct iw_context *ctx, int randomize, int rand_seed)
{
	ctx->randomize = randomize;
	ctx->random_seed = rand_seed;
}
