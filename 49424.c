static int __init sha256_ssse3_mod_init(void)
{
	/* test for SSSE3 first */
	if (cpu_has_ssse3)
		sha256_transform_asm = sha256_transform_ssse3;

#ifdef CONFIG_AS_AVX
	/* allow AVX to override SSSE3, it's a little faster */
	if (avx_usable()) {
#ifdef CONFIG_AS_AVX2
		if (boot_cpu_has(X86_FEATURE_AVX2) && boot_cpu_has(X86_FEATURE_BMI2))
			sha256_transform_asm = sha256_transform_rorx;
		else
#endif
			sha256_transform_asm = sha256_transform_avx;
	}
#endif

	if (sha256_transform_asm) {
#ifdef CONFIG_AS_AVX
		if (sha256_transform_asm == sha256_transform_avx)
			pr_info("Using AVX optimized SHA-256 implementation\n");
#ifdef CONFIG_AS_AVX2
		else if (sha256_transform_asm == sha256_transform_rorx)
			pr_info("Using AVX2 optimized SHA-256 implementation\n");
#endif
		else
#endif
			pr_info("Using SSSE3 optimized SHA-256 implementation\n");
		return crypto_register_shashes(algs, ARRAY_SIZE(algs));
	}
	pr_info("Neither AVX nor SSSE3 is available/usable.\n");

	return -ENODEV;
}
