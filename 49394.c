static int __init serpent_init(void)
{
	u64 xcr0;

	if (!cpu_has_avx || !cpu_has_osxsave) {
		printk(KERN_INFO "AVX instructions are not detected.\n");
		return -ENODEV;
	}

	xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);
	if ((xcr0 & (XSTATE_SSE | XSTATE_YMM)) != (XSTATE_SSE | XSTATE_YMM)) {
		printk(KERN_INFO "AVX detected but unusable.\n");
		return -ENODEV;
	}

	return crypto_register_algs(serpent_algs, ARRAY_SIZE(serpent_algs));
}
