static int __init crc32c_sparc64_mod_init(void)
{
	if (sparc64_has_crc32c_opcode()) {
		pr_info("Using sparc64 crc32c opcode optimized CRC32C implementation\n");
		return crypto_register_shash(&alg);
	}
	pr_info("sparc64 crc32c opcode not available.\n");
	return -ENODEV;
}
