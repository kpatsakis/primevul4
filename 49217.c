static int __init sha512_sparc64_mod_init(void)
{
	if (sparc64_has_sha512_opcode()) {
		int ret = crypto_register_shash(&sha384);
		if (ret < 0)
			return ret;

		ret = crypto_register_shash(&sha512);
		if (ret < 0) {
			crypto_unregister_shash(&sha384);
			return ret;
		}

		pr_info("Using sparc64 sha512 opcode optimized SHA-512/SHA-384 implementation\n");
		return 0;
	}
	pr_info("sparc64 sha512 opcode not available.\n");
	return -ENODEV;
}
