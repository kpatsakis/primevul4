static int ablk_ctr_init(struct crypto_tfm *tfm)
{
	return ablk_init_common(tfm, "__driver-ctr-aes-aesni");
}
