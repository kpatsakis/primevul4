static int crypto_cmac_digest_setkey(struct crypto_shash *parent,
				     const u8 *inkey, unsigned int keylen)
{
	unsigned long alignmask = crypto_shash_alignmask(parent);
	struct cmac_tfm_ctx *ctx = crypto_shash_ctx(parent);
	unsigned int bs = crypto_shash_blocksize(parent);
	__be64 *consts = PTR_ALIGN((void *)ctx->ctx, alignmask + 1);
	u64 _const[2];
	int i, err = 0;
	u8 msb_mask, gfmask;

	err = crypto_cipher_setkey(ctx->child, inkey, keylen);
	if (err)
		return err;

	/* encrypt the zero block */
	memset(consts, 0, bs);
	crypto_cipher_encrypt_one(ctx->child, (u8 *)consts, (u8 *)consts);

	switch (bs) {
	case 16:
		gfmask = 0x87;
		_const[0] = be64_to_cpu(consts[1]);
		_const[1] = be64_to_cpu(consts[0]);

		/* gf(2^128) multiply zero-ciphertext with u and u^2 */
		for (i = 0; i < 4; i += 2) {
			msb_mask = ((s64)_const[1] >> 63) & gfmask;
			_const[1] = (_const[1] << 1) | (_const[0] >> 63);
			_const[0] = (_const[0] << 1) ^ msb_mask;

			consts[i + 0] = cpu_to_be64(_const[1]);
			consts[i + 1] = cpu_to_be64(_const[0]);
		}

		break;
	case 8:
		gfmask = 0x1B;
		_const[0] = be64_to_cpu(consts[0]);

		/* gf(2^64) multiply zero-ciphertext with u and u^2 */
		for (i = 0; i < 2; i++) {
			msb_mask = ((s64)_const[0] >> 63) & gfmask;
			_const[0] = (_const[0] << 1) ^ msb_mask;

			consts[i] = cpu_to_be64(_const[0]);
		}

		break;
	}

	return 0;
}
