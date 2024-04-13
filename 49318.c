static int crc32_pclmul_cra_init(struct crypto_tfm *tfm)
{
	u32 *key = crypto_tfm_ctx(tfm);

	*key = 0;

	return 0;
}
