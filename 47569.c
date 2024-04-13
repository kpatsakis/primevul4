load_ucode_intel_bsp(void)
{
	u64 ramdisk_image, ramdisk_size;
	unsigned long initrd_start_early, initrd_end_early;
	struct ucode_cpu_info uci;
#ifdef CONFIG_X86_32
	struct boot_params *boot_params_p;

	boot_params_p = (struct boot_params *)__pa_nodebug(&boot_params);
	ramdisk_image = boot_params_p->hdr.ramdisk_image;
	ramdisk_size  = boot_params_p->hdr.ramdisk_size;
	initrd_start_early = ramdisk_image;
	initrd_end_early = initrd_start_early + ramdisk_size;

	_load_ucode_intel_bsp(
		(struct mc_saved_data *)__pa_nodebug(&mc_saved_data),
		(unsigned long *)__pa_nodebug(&mc_saved_in_initrd),
		initrd_start_early, initrd_end_early, &uci);
#else
	ramdisk_image = boot_params.hdr.ramdisk_image;
	ramdisk_size  = boot_params.hdr.ramdisk_size;
	initrd_start_early = ramdisk_image + PAGE_OFFSET;
	initrd_end_early = initrd_start_early + ramdisk_size;

	_load_ucode_intel_bsp(&mc_saved_data, mc_saved_in_initrd,
			      initrd_start_early, initrd_end_early,
			      &uci);
#endif
}
