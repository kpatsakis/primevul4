static int __init sha256_sparc64_mod_init(void)
{
	if (sparc64_has_sha256_opcode()) {
		int ret = crypto_register_shash(&sha224);
		if (ret < 0)
			return ret;

		ret = crypto_register_shash(&sha256);
		if (ret < 0) {
			crypto_unregister_shash(&sha224);
			return ret;
		}

		pr_info("Using sparc64 sha256 opcode optimized SHA-256/SHA-224 implementation\n");
		return 0;
	}
	pr_info("sparc64 sha256 opcode not available.\n");
	return -ENODEV;
}
