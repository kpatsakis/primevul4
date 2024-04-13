scan_microcode(unsigned long start, unsigned long end,
		struct mc_saved_data *mc_saved_data,
		unsigned long *mc_saved_in_initrd,
		struct ucode_cpu_info *uci)
{
	unsigned int size = end - start + 1;
	struct cpio_data cd;
	long offset = 0;
#ifdef CONFIG_X86_32
	char *p = (char *)__pa_nodebug(ucode_name);
#else
	char *p = ucode_name;
#endif

	cd.data = NULL;
	cd.size = 0;

	cd = find_cpio_data(p, (void *)start, size, &offset);
	if (!cd.data)
		return UCODE_ERROR;


	return get_matching_model_microcode(0, start, cd.data, cd.size,
					    mc_saved_data, mc_saved_in_initrd,
					    uci);
}
