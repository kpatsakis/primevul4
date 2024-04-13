static int __init crc32_pclmul_mod_init(void)
{

	if (!x86_match_cpu(crc32pclmul_cpu_id)) {
		pr_info("PCLMULQDQ-NI instructions are not detected.\n");
		return -ENODEV;
	}
	return crypto_register_shash(&alg);
}
