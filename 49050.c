static int __init aes_mod_init(void)
{
	if (!(elf_hwcap & HWCAP_AES))
		return -ENODEV;
	return crypto_register_alg(&ccm_aes_alg);
}
