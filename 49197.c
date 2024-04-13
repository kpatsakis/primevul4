static int __init sha1_sparc64_mod_init(void)
{
	if (sparc64_has_sha1_opcode()) {
		pr_info("Using sparc64 sha1 opcode optimized SHA-1 implementation\n");
		return crypto_register_shash(&alg);
	}
	pr_info("sparc64 sha1 opcode not available.\n");
	return -ENODEV;
}
