static int sha512_init(struct shash_desc *desc)
{
	struct s390_sha_ctx *ctx = shash_desc_ctx(desc);

	*(__u64 *)&ctx->state[0] = 0x6a09e667f3bcc908ULL;
	*(__u64 *)&ctx->state[2] = 0xbb67ae8584caa73bULL;
	*(__u64 *)&ctx->state[4] = 0x3c6ef372fe94f82bULL;
	*(__u64 *)&ctx->state[6] = 0xa54ff53a5f1d36f1ULL;
	*(__u64 *)&ctx->state[8] = 0x510e527fade682d1ULL;
	*(__u64 *)&ctx->state[10] = 0x9b05688c2b3e6c1fULL;
	*(__u64 *)&ctx->state[12] = 0x1f83d9abfb41bd6bULL;
	*(__u64 *)&ctx->state[14] = 0x5be0cd19137e2179ULL;
	ctx->count = 0;
	ctx->func = KIMD_SHA_512;

	return 0;
}
