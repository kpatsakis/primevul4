static int vmac_set_key(unsigned char user_key[], struct vmac_ctx_t *ctx)
{
	u64 in[2] = {0}, out[2];
	unsigned i;
	int err = 0;

	err = crypto_cipher_setkey(ctx->child, user_key, VMAC_KEY_LEN);
	if (err)
		return err;

	/* Fill nh key */
	((unsigned char *)in)[0] = 0x80;
	for (i = 0; i < sizeof(ctx->__vmac_ctx.nhkey)/8; i += 2) {
		crypto_cipher_encrypt_one(ctx->child,
			(unsigned char *)out, (unsigned char *)in);
		ctx->__vmac_ctx.nhkey[i] = be64_to_cpup(out);
		ctx->__vmac_ctx.nhkey[i+1] = be64_to_cpup(out+1);
		((unsigned char *)in)[15] += 1;
	}

	/* Fill poly key */
	((unsigned char *)in)[0] = 0xC0;
	in[1] = 0;
	for (i = 0; i < sizeof(ctx->__vmac_ctx.polykey)/8; i += 2) {
		crypto_cipher_encrypt_one(ctx->child,
			(unsigned char *)out, (unsigned char *)in);
		ctx->__vmac_ctx.polytmp[i] =
			ctx->__vmac_ctx.polykey[i] =
				be64_to_cpup(out) & mpoly;
		ctx->__vmac_ctx.polytmp[i+1] =
			ctx->__vmac_ctx.polykey[i+1] =
				be64_to_cpup(out+1) & mpoly;
		((unsigned char *)in)[15] += 1;
	}

	/* Fill ip key */
	((unsigned char *)in)[0] = 0xE0;
	in[1] = 0;
	for (i = 0; i < sizeof(ctx->__vmac_ctx.l3key)/8; i += 2) {
		do {
			crypto_cipher_encrypt_one(ctx->child,
				(unsigned char *)out, (unsigned char *)in);
			ctx->__vmac_ctx.l3key[i] = be64_to_cpup(out);
			ctx->__vmac_ctx.l3key[i+1] = be64_to_cpup(out+1);
			((unsigned char *)in)[15] += 1;
		} while (ctx->__vmac_ctx.l3key[i] >= p64
			|| ctx->__vmac_ctx.l3key[i+1] >= p64);
	}

	/* Invalidate nonce/aes cache and reset other elements */
	ctx->__vmac_ctx.cached_nonce[0] = (u64)-1; /* Ensure illegal nonce */
	ctx->__vmac_ctx.cached_nonce[1] = (u64)0;  /* Ensure illegal nonce */
	ctx->__vmac_ctx.first_block_processed = 0;

	return err;
}
