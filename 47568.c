void load_ucode_intel_ap(void)
{
	struct mc_saved_data *mc_saved_data_p;
	struct ucode_cpu_info uci;
	unsigned long *mc_saved_in_initrd_p;
	unsigned long initrd_start_addr;
#ifdef CONFIG_X86_32
	unsigned long *initrd_start_p;

	mc_saved_in_initrd_p =
		(unsigned long *)__pa_nodebug(mc_saved_in_initrd);
	mc_saved_data_p = (struct mc_saved_data *)__pa_nodebug(&mc_saved_data);
	initrd_start_p = (unsigned long *)__pa_nodebug(&initrd_start);
	initrd_start_addr = (unsigned long)__pa_nodebug(*initrd_start_p);
#else
	mc_saved_data_p = &mc_saved_data;
	mc_saved_in_initrd_p = mc_saved_in_initrd;
	initrd_start_addr = initrd_start;
#endif

	/*
	 * If there is no valid ucode previously saved in memory, no need to
	 * update ucode on this AP.
	 */
	if (mc_saved_data_p->mc_saved_count == 0)
		return;

	collect_cpu_info_early(&uci);
	load_microcode(mc_saved_data_p, mc_saved_in_initrd_p,
		       initrd_start_addr, &uci);
	apply_microcode_early(&uci, true);
}
