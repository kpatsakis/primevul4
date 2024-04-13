static void cpuid_mask(u32 *word, int wordnum)
{
	*word &= boot_cpu_data.x86_capability[wordnum];
}
