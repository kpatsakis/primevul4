static int __init crc32c_intel_mod_init(void)
{
	if (!x86_match_cpu(crc32c_cpu_id))
		return -ENODEV;
#ifdef CONFIG_X86_64
	if (cpu_has_pclmulqdq) {
		alg.update = crc32c_pcl_intel_update;
		alg.finup = crc32c_pcl_intel_finup;
		alg.digest = crc32c_pcl_intel_digest;
		set_pcl_breakeven_point();
	}
#endif
	return crypto_register_shash(&alg);
}
