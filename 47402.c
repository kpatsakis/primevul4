static int ablk_pcbc_init(struct crypto_tfm *tfm)
{
	return ablk_init_common(tfm, "fpu(pcbc(__driver-aes-aesni))");
}
