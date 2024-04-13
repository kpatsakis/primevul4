static int __init camellia_aesni_init(void)
{
	u64 xcr0;

	if (!cpu_has_avx2 || !cpu_has_avx || !cpu_has_aes || !cpu_has_osxsave) {
		pr_info("AVX2 or AES-NI instructions are not detected.\n");
		return -ENODEV;
	}

	xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);
	if ((xcr0 & (XSTATE_SSE | XSTATE_YMM)) != (XSTATE_SSE | XSTATE_YMM)) {
		pr_info("AVX2 detected but unusable.\n");
		return -ENODEV;
	}

	return crypto_register_algs(cmll_algs, ARRAY_SIZE(cmll_algs));
}
