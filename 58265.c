static void __init probe_page_size_mask(void)
{
#if !defined(CONFIG_KMEMCHECK)
	/*
	 * For CONFIG_KMEMCHECK or pagealloc debugging, identity mapping will
	 * use small pages.
	 * This will simplify cpa(), which otherwise needs to support splitting
	 * large pages into small in interrupt context, etc.
	 */
	if (boot_cpu_has(X86_FEATURE_PSE) && !debug_pagealloc_enabled())
		page_size_mask |= 1 << PG_LEVEL_2M;
#endif

	/* Enable PSE if available */
	if (boot_cpu_has(X86_FEATURE_PSE))
		cr4_set_bits_and_update_boot(X86_CR4_PSE);

	/* Enable PGE if available */
	if (boot_cpu_has(X86_FEATURE_PGE)) {
		cr4_set_bits_and_update_boot(X86_CR4_PGE);
		__supported_pte_mask |= _PAGE_GLOBAL;
	} else
		__supported_pte_mask &= ~_PAGE_GLOBAL;

	/* Enable 1 GB linear kernel mappings if available: */
	if (direct_gbpages && boot_cpu_has(X86_FEATURE_GBPAGES)) {
		printk(KERN_INFO "Using GB pages for direct mapping\n");
		page_size_mask |= 1 << PG_LEVEL_1G;
	} else {
		direct_gbpages = 0;
	}
}
