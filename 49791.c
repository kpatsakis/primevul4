static int padlock_sha256_init_nano(struct shash_desc *desc)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);

	*sctx = (struct sha256_state){
		.state = { SHA256_H0, SHA256_H1, SHA256_H2, SHA256_H3, \
				SHA256_H4, SHA256_H5, SHA256_H6, SHA256_H7},
	};

	return 0;
}
