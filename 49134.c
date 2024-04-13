 static int sha384_init(struct shash_desc *desc)
 {
	struct s390_sha_ctx *ctx = shash_desc_ctx(desc);

	*(__u64 *)&ctx->state[0] = 0xcbbb9d5dc1059ed8ULL;
	*(__u64 *)&ctx->state[2] = 0x629a292a367cd507ULL;
	*(__u64 *)&ctx->state[4] = 0x9159015a3070dd17ULL;
	*(__u64 *)&ctx->state[6] = 0x152fecd8f70e5939ULL;
	*(__u64 *)&ctx->state[8] = 0x67332667ffc00b31ULL;
	*(__u64 *)&ctx->state[10] = 0x8eb44a8768581511ULL;
	*(__u64 *)&ctx->state[12] = 0xdb0c2e0d64f98fa7ULL;
	*(__u64 *)&ctx->state[14] = 0x47b5481dbefa4fa4ULL;
	ctx->count = 0;
	ctx->func = KIMD_SHA_512;

	return 0;
}
