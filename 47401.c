static int ablk_ecb_init(struct crypto_tfm *tfm)
{
	return ablk_init_common(tfm, "__driver-ecb-aes-aesni");
}
