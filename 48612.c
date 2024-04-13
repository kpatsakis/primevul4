static u64 vmac(unsigned char m[], unsigned int mbytes,
			const unsigned char n[16], u64 *tagl,
			struct vmac_ctx_t *ctx)
{
	u64 *in_n, *out_p;
	u64 p, h;
	int i;

	in_n = ctx->__vmac_ctx.cached_nonce;
	out_p = ctx->__vmac_ctx.cached_aes;

	i = n[15] & 1;
	if ((*(u64 *)(n+8) != in_n[1]) || (*(u64 *)(n) != in_n[0])) {
		in_n[0] = *(u64 *)(n);
		in_n[1] = *(u64 *)(n+8);
		((unsigned char *)in_n)[15] &= 0xFE;
		crypto_cipher_encrypt_one(ctx->child,
			(unsigned char *)out_p, (unsigned char *)in_n);

		((unsigned char *)in_n)[15] |= (unsigned char)(1-i);
	}
	p = be64_to_cpup(out_p + i);
	h = vhash(m, mbytes, (u64 *)0, &ctx->__vmac_ctx);
	return le64_to_cpu(p + h);
}
