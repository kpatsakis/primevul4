static int __init init(void)
{
	u64 xcr0;

	if (!cpu_has_avx2 || !cpu_has_osxsave) {
		pr_info("AVX2 instructions are not detected.\n");
		return -ENODEV;
	}

	xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);
	if ((xcr0 & (XSTATE_SSE | XSTATE_YMM)) != (XSTATE_SSE | XSTATE_YMM)) {
		pr_info("AVX detected but unusable.\n");
		return -ENODEV;
	}

	return crypto_register_algs(srp_algs, ARRAY_SIZE(srp_algs));
}
