static void ctr_crypt_final(struct crypto_sparc64_aes_ctx *ctx,
			    struct blkcipher_walk *walk)
{
	u8 *ctrblk = walk->iv;
	u64 keystream[AES_BLOCK_SIZE / sizeof(u64)];
	u8 *src = walk->src.virt.addr;
	u8 *dst = walk->dst.virt.addr;
	unsigned int nbytes = walk->nbytes;

	ctx->ops->ecb_encrypt(&ctx->key[0], (const u64 *)ctrblk,
			      keystream, AES_BLOCK_SIZE);
	crypto_xor((u8 *) keystream, src, nbytes);
	memcpy(dst, keystream, nbytes);
	crypto_inc(ctrblk, AES_BLOCK_SIZE);
}
